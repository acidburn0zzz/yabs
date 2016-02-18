// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;
extern crate rustc_serialize;
extern crate walkdir;
extern crate rpf;

use toml::decode;
use error::YabsError;
use rpf::*;
use ext::*;
use rustc_serialize::{Decodable, Decoder, Encodable, Encoder, json};
use walkdir::{WalkDir, WalkDirIterator, DirEntry};

use std::ffi::OsStr;
use std::fs::File;
use std::io::{Read, Write};
use std::process::Command;


// Profile has descriptions that describe build instructions (proj_desc),
// install instructions (inst_desc), and documentation instructions (doc_desc).
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct Profile {
    name: String,
    proj_desc: Option<ProjDesc>,
    inst_desc: Option<InstallDesc>,
    doc_desc: Option<DocDesc>,
}

// A build file could have multiple `Profile`s
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone)]
pub struct BuildFile {
    profiles: Vec<Profile>,
}

impl BuildFile {
    // Creates a `Profiles` from a toml file.
    pub fn from_file(file: &str) -> Result<BuildFile, Vec<YabsError>> {
        let mut build_file: BuildFile = Default::default();
        parse_toml_file(file)
            .and_then(|toml| {
                for (key, table) in toml {
                    let mut profile: Profile = Default::default();
                    profile.name = key.clone();
                    match table {
                        toml::Value::Table(inner_table) => {
                            for (key, table) in inner_table {
                                match key.as_ref() {
                                    "project" => {
                                        profile.proj_desc = ProjDesc::from_toml_table(table).ok()
                                    }
                                    "install" => {
                                        profile.inst_desc = InstallDesc::from_toml_table(table).ok()
                                    }
                                    "doc" => {
                                        profile.doc_desc = DocDesc::from_toml_table(table).ok()
                                    }
                                    _ => (),
                                }
                            }
                        }
                        _ => (),
                    };
                    build_file.profiles.push(profile);
                }
                Ok(build_file)
            })
            .map_err(|err| err)
    }

    pub fn print_as_json(&self) {
        for profile in &self.profiles {
            profile.print_json();
        }
    }

    pub fn print_available_profiles(&self) {
        for profile in &self.profiles {
            print!("{} ", profile.name);
        }
        print!("\n");
    }

    // Prints a profile with name `name` in build file as JSON
    pub fn print_profile_as_json(&self, name: String) {
        for profile in &self.profiles {
            if profile.name == name {
                profile.print_json();
            }
        }
    }

    // Generate a Makefile using from a profile with name `name`
    pub fn gen_make(&self, name: String) -> Result<(), YabsError> {
        if let Some(index) = self.profiles.iter().position(|ref profile| profile.name == name) {
            try!(try!(File::create("Makefile")).write_all(try!(self.profiles[index.clone()]
                                                                   .clone()
                                                                   .proj_desc
                                                                   .unwrap_or(ProjDesc::new())
                                                                   .gen_make())
                                                              .as_bytes()));
            Ok(())
        } else {
            Err(YabsError::NoDesc(name))
        }
    }

    pub fn build(&self, name: String, jobs: i32) -> Result<(), YabsError> {
        if let Some(index) = self.profiles.iter().position(|ref profile| profile.name == name) {
            try!(self.profiles[index.clone()].clone().proj_desc.unwrap().build_bin(jobs));
            Ok(())
        } else {
            Err(YabsError::NoDesc(name))
        }
    }

    pub fn print_sources(&mut self) -> Result<(), YabsError> {
        for profile in &mut self.profiles {
            if let &mut Some(ref mut proj) = &mut profile.proj_desc {
                println!("{}", profile.name.bold());
                try!(proj.gen_file_list());
                if let Some(set_sources) = proj.src.as_ref() {
                    for src in set_sources {
                        println!("{}", src);
                    }
                }
            }
        }
        Ok(())
    }
}

#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct ProjDesc {
    name: Option<String>,
    target: Option<String>,
    lang: Option<String>,
    os: Option<String>,
    version: Option<String>,
    compiler: Option<String>,
    src: Option<Vec<String>>,
    libs: Option<Vec<String>>,
    lib_dir: Option<Vec<String>>,
    inc: Option<Vec<String>>,
    inc_dir: Option<Vec<String>>,
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

impl ProjDesc {
    fn concat_clean(&self) -> String {
        self.prepend_op_vec(&self.clean, "".to_string())
    }

    fn is_in_ignore(&self, entry: &DirEntry) -> bool {
        if let Some(ignore) = self.ignore.as_ref() {
            for path in ignore {
                let entry = entry.path().as_string();
                if entry.find(path).is_some() {
                    return true;
                }
            }
        }
        false
    }

    fn gen_file_list(&mut self) -> Result<(), YabsError> {
        if self.src.is_some() {
            return Ok(());
        }
        let mut sources = Vec::new();
        let walk_dir = WalkDir::new(".").into_iter();
        for entry in walk_dir.filter_entry(|e| !&self.is_in_ignore(e)) {
            let entry = try!(entry);
            if entry.path().is_file() {
                let file_ext = entry.path().extension().unwrap_or(OsStr::new(""));
                if let Some(ext) = file_ext.to_str() {
                    if let Some(lang) = self.lang.as_ref() {
                        if ext == lang {
                            let mut entry = entry.path().as_string();
                            if entry.len() > 2 {
                                entry.remove(0);
                                entry.remove(0);
                            }
                            sources.push(entry);
                        }
                    }
                }
            }
        }
        self.src = Some(sources);
        Ok(())
    }

    // Concatenates a vector of strings `list`, prepending each entry with `prepend`
    fn prepend_op_vec(&self, list: &Option<Vec<String>>, prepend: String) -> String {
        let mut horrid_string = String::new();
        if let Some(items) = list.as_ref() {
            if let Some(split_last) = items.split_last() {
                for sub_item in split_last.1 {
                    if self.is_command(&sub_item) {
                        horrid_string.push_str(&format!("{} ", sub_item));
                    } else {
                        horrid_string.push_str(&format!("{}{} ", prepend, sub_item));
                    }
                }
                if self.is_command(&split_last.0) {
                    horrid_string.push_str(&format!("{} ", split_last.0));
                } else {
                    horrid_string.push_str(&format!("{}{}", prepend, split_last.0.clone()));
                }
            }
        }
        return horrid_string;
    }

    // Any string that starts with the character "`" is regarded as a command
    fn is_command(&self, string: &String) -> bool {
        if string.starts_with("`") {
            return true;
        } else {
            return false;
        }
    }

    fn gen_make_lib_dir_list(&self) -> String {
        return self.prepend_op_vec(&self.lib_dir, "-L".to_owned());
    }

    fn gen_make_lib_list(&self) -> String {
        return self.prepend_op_vec(&self.libs, "-l".to_string());
    }

    fn gen_make_cflags_list(&self) -> String {
        return self.prepend_op_vec(&self.cflags, "-".to_string());
    }

    fn gen_make_lflags_list(&self) -> String {
        return self.prepend_op_vec(&self.lflags, "-".to_string());
    }

    fn gen_make_src_list(&self) -> String {
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
                            horrid_string.push_str(&format!("\t{} \\\n", src));
                        }
                        horrid_string.push_str(&format!("\t{}\n", split_last.0));
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
                            horrid_string.push_str(&format!("\t{} \\\n", src));
                        }
                        horrid_string.push_str(&format!("{}", split_last.0));
                    }
                }
            } else {
                for obj in parsed_obj_list {
                    horrid_string.push_str(&format!("\t{}\n", obj));
                }
            }
        }
        return horrid_string;
    }

    fn gen_make_src_deps(&self) -> String {
        let mut horrid_string = String::new();
        let mut lang = self.lang.clone().unwrap_or("cpp".to_owned());
        lang.insert(0, '.');
        if let Some(source_list) = self.src.as_ref() {
            for src in source_list {
                horrid_string.push_str(&format!("{0}: {1}\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \
                                                 {0} {1}\n\n",
                                                src.replace(&lang, ".o"),
                                                src));
            }
        }
        return horrid_string;
    }

    fn gen_make_inc_list(&self) -> String {
        return self.prepend_op_vec(&self.inc, "-I".to_string());
    }

    fn gen_make(&mut self) -> Result<String, YabsError> {
        try!(self.gen_file_list());
        let mut target_string = String::new();
        if let Some(static_lib) = self.static_lib {
            if static_lib == true {
                target_string = format!("$(TARGET): $(OBJ)\n\
                \t$(AR) $(ARFLAGS) $(TARGET) $(OBJ)\n\n");
            } else {
                target_string = format!("$(TARGET): $(OBJ)\n\
                \t$(CC) $(LFLAGS) -o $(TARGET) $(OBJ) $(LIBS)\n\n");
            }
        };
        Ok(format!(
            "INSTALL\t= /usr/bin/env install\n\
                AR\t= {ar}\n\
                ARFLAGS\t= {ar_flags}\n\
                DEST\t=\n\
                PREFIX\t=\n\
                CC\t= {compiler}\n\
                BINDIR\t=\n\
                LIBDIR\t=\n\
                TARGET\t= {target}\n\
                LINK\t=\n\
                CFLAGS\t= {cflags}\n\
                LFLAGS\t=\n\
                LIBS\t= {libs}\n\
                INCDIR\t= {incdir}\n\
                LIBDIR\t=\n\
                CLEAN\t=\n\
                DEL\t= rm -f\n\
                {srcs}\n\n\
                first: all\n\n\
                .PHONY: doc clean\n\n\
                .SUFFIXES: .o .c .cpp .cc .cxx .C\n\n\
                .cpp.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                .cc.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                .cxx.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                .C.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                .c.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                all: $(TARGET)\n\n\
                {target_command}\
                {dep_list}\n\
                clean:\n\
                \t$(DEL) $(OBJ)\n\
                \t$(DEL) {target}\n\
                \t$(DEL) {clean_list}\n\
                ",
                compiler = &self.compiler.as_ref().unwrap_or(&"gcc".to_owned()),
                ar = &self.ar.as_ref().unwrap_or(&"/usr/bin/env ar".to_owned()),
                ar_flags = &self.arflags.as_ref().unwrap_or(&"rcs".to_owned()),
                target = &self.target.as_ref().unwrap_or(&"a".to_owned()),
                target_command = target_string,
                cflags = &self.gen_make_cflags_list(),
                libs = &self.gen_make_lib_list(),
                incdir = &self.gen_make_inc_list(),
                srcs = &self.gen_make_src_list(),
                dep_list = &self.gen_make_src_deps(),
                clean_list = &self.concat_clean()))
    }

    fn build_bin(&mut self, jobs: i32) -> Result<(), YabsError> {
        try!(self.gen_file_list());
        if let Some(src_list) = self.src.as_ref() {
            let mut lang = self.lang.clone().unwrap_or("cpp".to_owned());
            lang.insert(0, '.');
            let mut cmd_string = String::new();
            let mut obj_vec = Vec::new();
            for src in src_list {
                cmd_string = format!("{cc} -c {cflag} {inc} -o {object} {source}",
                                         cc = self.compiler.as_ref().unwrap_or(&"gcc".to_owned()),
                                         cflag = self.gen_make_cflags_list(),
                                         inc = self.gen_make_inc_list(),
                                         source = src,
                                         object = src.replace(&lang, ".o"),
                                         );
                obj_vec.push(src.replace(&lang, ".o"));
                let mut command = try!(Command::new("sh")
                                           .arg("-c")
                                           .arg(&cmd_string)
                                           .spawn());
                println!("{}", cmd_string);
                let status = try!(command.wait());
            }
            cmd_string = format!("{cc} {lflags} -o {target} {obj_list} {lib_dir} {libs}",
                                 cc = self.compiler.as_ref().unwrap_or(&"gcc".to_owned()),
                                 lflags = self.gen_make_lflags_list(),
                                 target = self.target.as_ref().unwrap_or(&"a".to_owned()),
                                 obj_list = self.prepend_op_vec(&Some(obj_vec.clone()),
                                                                "".to_owned()),
                                 lib_dir = self.gen_make_lib_dir_list(),
                                 libs = self.gen_make_lib_list());
            if let Some(static_lib) = self.static_lib {
                if static_lib == true {
                    cmd_string = format!("{ar} {ar_flags} {target} {obj_list}",
                                         ar = self.ar.as_ref().unwrap_or(&"/usr/bin/env ar".to_owned()),
                                         ar_flags = self.arflags.as_ref().unwrap_or(&"rcs".to_owned()),
                                         target = self.target.as_ref().unwrap_or(&"a".to_owned()),
                                         obj_list = self.prepend_op_vec(&Some(obj_vec), "".to_owned()),
                                         );
                }
            };
            let mut command = try!(Command::new("sh")
                                       .arg("-c")
                                       .arg(&cmd_string)
                                       .spawn());
            println!("{}", cmd_string);
            let status = try!(command.wait());
        };
        Ok(())
    }
}

// Descibe how to install this project
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct InstallDesc {
    prefix: Option<String>,
}

// Generate documentation for this project
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct DocDesc {
    doc: Option<String>,
}

// General trait for any description.
pub trait Desc<T> {
    fn new() -> T;
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>>;
    fn from_toml_table(table: toml::Value) -> Result<T, YabsError>;
    fn print_json(&self);
}

impl<T: Decodable + Encodable + Default> Desc<T> for T {
    // Creates an empty description using `Default`
    fn new() -> T {
        Default::default()
    }

    // Propogates a description from a toml file with the key `name`
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>> {
        parse_toml_file(file).and_then(|toml| {
            toml.get(name)
                .ok_or(vec![YabsError::NoDesc(name.to_owned())])
                .and_then(|desc| {
                    Decodable::decode(&mut toml::Decoder::new(desc.clone()))
                        .map_err(|err| vec![YabsError::TomlDecode(err)])
                })
        })
    }

    fn from_toml_table(table: toml::Value) -> Result<T, YabsError> {
        Ok(try!(Decodable::decode(&mut toml::Decoder::new(table.clone()))))
    }

    fn print_json(&self) {
        println!("{}", json::as_pretty_json(&self));
    }
}
