extern crate serde;
extern crate toml;
extern crate walkdir;
extern crate regex;

use error::YabsError;
use ext::*;
use regex::Regex;
use std::collections::BTreeMap;

use std::fs::metadata;
use std::path::PathBuf;
use std::time::SystemTime;
use walkdir::{DirEntry, WalkDir, WalkDirIterator};

#[derive(Debug, Default, Deserialize, Serialize, Clone, PartialEq)]
pub struct Binary {
    // Name of target binary
    name: String,
    // Path to entry point
    path: PathBuf,
}

impl Binary {
    pub fn name(&self) -> String {
        self.name.clone()
    }

    pub fn path(&self) -> PathBuf {
        self.path.clone()
    }
}

#[derive(Debug, Default, Clone, PartialEq, PartialOrd, Eq, Ord)]
pub struct Target {
    source: PathBuf,
    object: PathBuf,
}

impl Target {
    pub fn new(src: PathBuf, obj: PathBuf) -> Target {
        Target {
            source: src,
            object: obj,
        }
    }

    pub fn source(&self) -> PathBuf {
        self.source.clone()
    }

    pub fn object(&self) -> PathBuf {
        self.object.clone()
    }
}

#[derive(Debug, Default, Deserialize, Serialize, Clone, PartialEq)]
pub struct Library {
    name: String,
    path: PathBuf,
}

impl Library {
    pub fn name(&self) -> String {
        self.name.clone()
    }

    pub fn path(&self) -> PathBuf {
        self.path.clone()
    }
}

#[derive(Debug, Default, Deserialize, Serialize, Clone, PartialEq)]
pub struct ProjectDesc {
    pub name: Option<String>,
    #[serde(rename = "file-extensions")]
    pub file_exts: Vec<String>,
    pub version: Option<String>,
    pub compiler: Option<String>,
    pub src: Option<Vec<PathBuf>>,
    #[serde(rename = "libraries")]
    pub libs: Option<Vec<String>>,
    #[serde(rename = "library-directories")]
    pub lib_dir: Option<Vec<String>>,
    pub include: Option<Vec<String>>,
    #[serde(rename = "compiler-flags")]
    pub compiler_flags: Option<Vec<String>>,
    #[serde(rename = "explicit-cflags")]
    pub explicit_cflags: Option<String>,
    #[serde(rename = "linker-flags")]
    pub lflags: Option<Vec<String>>,
    pub ignore: Option<Vec<String>>,
    #[serde(rename = "before-script")]
    pub before_script: Option<Vec<String>>,
    #[serde(rename = "after-script")]
    pub after_script: Option<Vec<String>>,
    pub ar: Option<String>,
    pub arflags: Option<String>,

    // Serde, please don't touch this, ty
    #[serde(skip_serializing)]
    #[serde(skip_deserializing)]
    pub object_list: Vec<PathBuf>,

    #[serde(skip_serializing)]
    #[serde(skip_deserializing)]
    pub file_mod_map: BTreeMap<Target, SystemTime>,
}

impl ProjectDesc {
    // TODO: Maybe have some kind of regex here to allow '*' and friends?
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

    // We have to do this as we regard entries that start with '`' as commands
    // and don't want to prepend them
    pub fn libs_as_string(&self) -> String {
        let mut lib_list: Vec<String> = Vec::new();
        let mut cmd_list: Vec<String> = Vec::new();
        if let Some(libs) = self.libs.as_ref() {
            for lib in libs {
                if self.is_command(&lib) {
                    cmd_list.push(lib.clone());
                } else {
                    lib_list.push(lib.clone());
                }
            }
        }
        lib_list = lib_list.prepend_each("-l");
        lib_list.extend(cmd_list);
        lib_list.join(" ")
    }

    // We don't care if a path is invalid UTF-8.
    // ! Potentially spoopy !
    // We can optionally exclude an object with source path `exclude`
    pub fn object_list_as_string(&self, exclude: Option<Vec<Binary>>) -> Result<String, YabsError> {
        let mut obj_str_list: Vec<String> = Vec::new();
        for target in self.file_mod_map.keys() {
            if let Some(ref exclude) = exclude {
                for bin in exclude {
                    if target.source.canonicalize()? != bin.path().canonicalize()? {
                        if let Some(obj_str) = target.object.to_str() {
                            obj_str_list.push(format!("\"{}\"", obj_str.to_owned()));
                        }
                    }
                }
            } else {
                if let Some(obj_str) = target.object.to_str() {
                    obj_str_list.push(format!("\"{}\"", obj_str.to_owned()));
                }
            }
        }
        Ok(obj_str_list.join(" "))
    }

    // Sources are found with a regular expression "(.*)\.[EXTENSIONS]+$"
    // where extensions would be the joined list given in 'file-extensions' in
    // a configuration file. The filename is then stored in the "(.*)" group
    pub fn find_source_files(&mut self) -> Result<(), YabsError> {
        // If sources are listed don't walk the current directory for files.
        if self.src.is_some() {
            let regex = Regex::new(&format!("(.*)\\.[{}]+$", self.file_exts.join("|")))?;
            for entry in self.src.clone().unwrap() {
                if let Some(src_str) = entry.clone().to_str() {
                    &self.file_mod_map
                         .insert(Target::new(entry.clone(),
                                             PathBuf::from(String::from(regex.replace(src_str, "${1}.o")))),
                                 metadata(&entry)?.modified()?);
                }
            }
        } else {
            &self.walk_current_dir()?;
        }
        Ok(())
    }

    // Same regex used as `find_source_files`
    fn walk_current_dir(&mut self) -> Result<(), YabsError> {
        let regex = Regex::new(&format!("(.*)\\.[{}]+$", self.file_exts.join("|")))?;
        let mut sources = Vec::new();
        let mut file_mod_map = BTreeMap::new();
        let walk_dir = WalkDir::new(".").into_iter();
        for entry in walk_dir.filter_entry(|e| !&self.is_in_ignore(e)) {
            let entry = entry?;
            if entry.path().is_file() {
                if let Some(filename_str) = entry.path().to_str() {
                    if regex.is_match(filename_str) {
                        file_mod_map.insert(Target::new(entry.path().to_path_buf(),
                                                        PathBuf::from(String::from(regex.replace(filename_str, "${1}.o")))),
                                            metadata(entry.path())?.modified()?);
                    }
                }
            }
        }
        sources.sort();
        self.src = Some(sources);
        self.file_mod_map = file_mod_map;
        Ok(())
    }

    // Any string that starts and ends with the character "`" is regarded as a
    // command
    pub fn is_command(&self, string: &String) -> bool {
        string.starts_with("`") && string.ends_with("`")
    }

    pub fn run_script(&self, script: &Option<Vec<String>>) -> Result<(), YabsError> {
        if let Some(script) = script.as_ref() {
            for cmd in script {
                run_cmd(cmd)?;
            }
        }
        Ok(())
    }
}
