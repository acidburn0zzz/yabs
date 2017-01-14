extern crate toml;
extern crate walkdir;

use error::YabsError;
use ext::*;
use desc::desc::*;
use walkdir::{WalkDir, WalkDirIterator, DirEntry};

use std::ffi::OsStr;

#[derive(Debug,Default,Deserialize,Serialize,Clone,PartialEq)]
pub struct ProjectDesc {
    name: Option<String>,
    target: Option<Vec<String>>,
    lang: Option<String>,
    os: Option<String>,
    version: Option<String>,
    compiler: Option<String>,
    src: Option<Vec<String>>,
    libs: Option<Vec<String>>,
    lib_dir: Option<Vec<String>>,
    inc: Option<Vec<String>>,
    cflags: Option<Vec<String>>,
    explicit_cflags: Option<String>,
    lflags: Option<Vec<String>>,
    ignore: Option<Vec<String>>,
    before_script: Option<Vec<String>>,
    after_script: Option<Vec<String>>,
    static_lib: Option<bool>,
    ar: Option<String>,
    arflags: Option<String>,
    clean: Option<Vec<String>>,
}

impl ProjectDesc {
    pub fn get_src(&self) -> Option<Vec<String>> {
        return self.src.clone();
    }

    pub fn is_in_ignore(&self, entry: &DirEntry) -> bool {
        if let Some(ignore) = self.ignore.as_ref() {
            for path in ignore {
                if let Some(entry_str) = entry.path().to_str() {
                    if entry_str.find(path).is_some() {
                        return true;
                    }
                }
            }
        }
        false
    }

    pub fn gen_file_list(&mut self) -> Result<(), YabsError> {
        // If sources are listed don't scan for files
        if self.src.is_some() {
            return Ok(());
        }
        let mut sources = Vec::new();
        let walk_dir = WalkDir::new(".").into_iter();
        for entry in walk_dir.filter_entry(|e| !&self.is_in_ignore(e)) {
            let entry = entry?;
            if entry.path().is_file() {
                let file_ext = entry.path().extension().unwrap_or(OsStr::new(""));
                if let Some(ext) = file_ext.to_str() {
                    if let Some(lang) = self.lang.as_ref() {
                        if ext == lang {
                            if let Some(entry_str) = entry.path().to_str() {
                                sources.push(entry_str.to_owned());
                            }
                        }
                    }
                }
            }
        }
        sources.sort();
        self.src = Some(sources);
        Ok(())
    }

    // Concatenates a vector of strings `list`, prepending each entry with `prepend`
    pub fn prepend_op_vec(&self, list: &Option<Vec<String>>, prepend: &str) -> String {
        let mut horrid_string = String::new();
        if let Some(items) = list.as_ref() {
            if let Some(split_last) = items.split_last() {
                for sub_item in split_last.1 {
                    if self.is_command(&sub_item) {
                        horrid_string.push_str(&format!("{} ", sub_item));
                    } else {
                        horrid_string.push_str(&format!("{}{} ", prepend.to_owned(), sub_item));
                    }
                }
                if self.is_command(&split_last.0) {
                    horrid_string.push_str(&format!("{} ", split_last.0));
                } else {
                    horrid_string.push_str(&format!("{}{}", prepend.to_owned(), split_last.0.clone()));
                }
            }
        }
        return horrid_string;
    }

    // Any string that starts and ends with the character "`" is regarded as a command
    pub fn is_command(&self, string: &String) -> bool {
        if string.starts_with("`") && string.ends_with("`") {
            return true;
        } else {
            return false;
        }
    }

    pub fn gen_target_list(&self) -> Target {
        let mut all = String::new();
        let mut target_str = String::new();
        if let Some(targets) = self.target.clone() {
            all = targets.concat();
            if let Some(static_lib) = self.static_lib {
                if targets.len() == 1 {
                    if static_lib {
                        target_str = format!("$(TARGET): $(OBJ)\n\
                            \t$(AR) $(ARFLAGS) $(TARGET) $(OBJ)\n\n");
                    } else {
                        target_str = format!("$(TARGET): $(OBJ)\n\
                            \t$(CC) $(LFLAGS) -o $(TARGET) $(OBJ) $(LIBDIR) $(LIBS)\n\n");
                    }
                } else {
                    if static_lib {
                        for target in targets {
                            target_str.push_str(&format!("{0}: $(OBJ)\n\
                                    \t$(AR) $(ARFLAGS) {0} $(OBJ)\n\n", target));
                        }
                    } else {
                        for target in targets {
                            target_str.push_str(&format!("{0}: $(OBJ)\n\
                                    \t$(CC) $(LFLAGS) -o {0} $(OBJ) $(LIBDIR) $(LIBS)\n\n", target));
                        }
                    }
                }
            }
        };
        Target { target: target_str, all: all }
    }

    fn gen_lib_dir_list(&self) -> String {
        return self.prepend_op_vec(&self.lib_dir, "-L");
    }

    fn gen_lib_list(&self) -> String {
        return self.prepend_op_vec(&self.libs, "-l");
    }

    fn gen_cflags_list(&self) -> String {
        return self.prepend_op_vec(&self.cflags, "-");
    }

    fn gen_lflags_list(&self) -> String {
        return self.prepend_op_vec(&self.lflags, "-");
    }

    fn gen_src_list(&self) -> String {
        let mut horrid_string: String = "SRC\t= ".to_owned();
        let mut lang = self.lang.clone().unwrap_or("cpp".to_owned());
        lang.insert(0, '.');
        if let Some(source_list) = self.src.as_ref() {
            // Multiple sources
            if source_list.len() > 1 {
                if let Some(split_first) = source_list.split_first() {
                    horrid_string.push_str(&format!("{} \\\n", split_first.0));
                    if let Some(split_last) = split_first.1.clone().split_last() {
                        for src in split_last.1 {
                            horrid_string.push_str(&format!("\t  {} \\\n", src));
                        }
                        horrid_string.push_str(&format!("\t  {}\n", split_last.0));
                    }
                }
            // One source file
            } else {
                for src in source_list {
                    horrid_string.push_str(&format!("{}\n", src));
                }
            }
            let mut parsed_obj_list = Vec::new();
            for obj in source_list {
                parsed_obj_list.push(obj.replace(&lang, ".o"));
            }
            horrid_string.push_str("OBJ\t= ");
            if parsed_obj_list.len() > 1 {
                if let Some(split_first) = parsed_obj_list.split_first() {
                    horrid_string.push_str(&format!("{} \\\n", split_first.0));
                    if let Some(split_last) = split_first.1.clone().split_last() {
                        for src in split_last.1 {
                            horrid_string.push_str(&format!("\t  {} \\\n", src));
                        }
                        horrid_string.push_str(&format!("\t  {}", split_last.0));
                    }
                }
            } else {
                for obj in parsed_obj_list {
                    horrid_string.push_str(&format!("{}\n", obj));
                }
            }
        }
        return horrid_string;
    }

    fn gen_inc_list(&self) -> String {
        return self.prepend_op_vec(&self.inc, "-I");
    }

    pub fn gen_make(&mut self) -> Result<String, YabsError> {
        self.gen_file_list()?;
        let target = &self.gen_target_list();
        Ok(format!(
            "INSTALL\t= /usr/bin/env install\n\
                AR\t= {ar}\n\
                ARFLAGS\t= {ar_flags}\n\
                DEST\t=\n\
                PREFIX\t=\n\
                CC\t= {compiler}\n\
                BINDIR\t=\n\
                TARGET\t= {all}\n\
                LINK\t= {compiler}\n\
                CFLAGS\t= {cflags}\n\
                LFLAGS\t=\n\
                LIBS\t= {libs}\n\
                INCDIR\t= {incdir}\n\
                LIBDIR\t= {lib_dir}\n\
                CLEAN\t=\n\
                DEL\t= rm -f\n\
                {srcs}\n\n\
                first: all\n\n\
                .PHONY: doc clean\n\n\
                all: {all}\n\n\
                {target_command}\
                %.o: %.{lang}\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o $@ $<\n\
                clean:\n\
                \t$(DEL) $(OBJ)\n\
                \t$(DEL) {all}\n\
                \t$(DEL) {clean_list}\n\
                ",
                compiler = &self.compiler.as_ref().unwrap_or(&"gcc".to_owned()),
                ar = &self.ar.as_ref().unwrap_or(&"/usr/bin/env ar".to_owned()),
                ar_flags = &self.arflags.as_ref().unwrap_or(&"rcs".to_owned()),
                all = target.all,
                target_command = target.target,
                cflags = &self.gen_cflags_list(),
                libs = &self.gen_lib_list(),
                incdir = &self.gen_inc_list(),
                lib_dir = &self.gen_lib_dir_list(),
                srcs = &self.gen_src_list(),
                lang = &self.lang.clone().unwrap_or("c".to_owned()),
                clean_list = &self.clean.clone().unwrap_or(vec![]).concat()))
    }

    pub fn run_script(&self, script: &Option<Vec<String>>) -> Result<(), YabsError> {
        if let Some(script) = script.as_ref() {
            for cmd in script {
                run_cmd(cmd.to_owned())?;
            }
        }
        Ok(())
    }

    pub fn build_bin(&mut self) -> Result<(), YabsError> {
        self.run_script(&self.before_script)?;
        self.gen_file_list()?;
        if let Some(src_list) = self.src.as_ref() {
            let mut lang = self.lang.clone().unwrap_or("cpp".to_owned());
            lang.insert(0, '.');
            let mut cmd_string;
            let mut obj_vec = Vec::new();
            for src in src_list {
                cmd_string = format!("{cc} -c {cflag} {inc} -o {object} {source}",
                                         cc = self.compiler.as_ref().unwrap_or(&"gcc".to_owned()),
                                         cflag = self.gen_cflags_list(),
                                         inc = self.gen_inc_list(),
                                         source = src,
                                         object = src.replace(&lang, ".o"),
                                         );
                run_cmd(cmd_string)?;
                obj_vec.push(src.replace(&lang, ".o"));
            }
            for target in self.target.clone().unwrap_or(vec!["a".to_owned()]) {
                if self.static_lib.unwrap_or(false) == true {
                    cmd_string = format!("{ar} {ar_flags} {target} {obj_list}",
                                                 ar = self.ar.as_ref().unwrap_or(&"/usr/bin/env ar".to_owned()),
                                                 ar_flags = self.arflags.as_ref().unwrap_or(&"rcs".to_owned()),
                                                 target = target,
                                                 obj_list = obj_vec.concat(),
                                                 );
                    run_cmd(cmd_string)?;
                } else {
                    cmd_string = format!("{cc} {lflags} -o {target} {obj_list} {lib_dir} {libs}",
                                                 cc = self.compiler.as_ref().unwrap_or(&"gcc".to_owned()),
                                                 lflags = self.gen_lflags_list(),
                                                 target = target,
                                                 obj_list = &self.prepend_op_vec(&Some(obj_vec.clone()), " "),
                                                 lib_dir = self.gen_lib_dir_list(),
                                                 libs = self.gen_lib_list());
                    run_cmd(cmd_string)?;
                }
            };
        };
        self.run_script(&self.after_script)?;
        Ok(())
    }
}
