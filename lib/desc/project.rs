extern crate serde;
extern crate toml;
extern crate walkdir;

use error::YabsError;
use ext::*;
use std::collections::BTreeMap;

use std::ffi::OsStr;
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
    path: PathBuf,
}

impl Library {
    pub fn path(&self) -> PathBuf {
        self.path.clone()
    }
}

#[derive(Debug, Default, Deserialize, Serialize, Clone, PartialEq)]
pub struct ProjectDesc {
    pub name: Option<String>,
    pub lang: String,
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
                            obj_str_list.push(obj_str.to_owned());
                        }
                    }
                }
            } else {
                if let Some(obj_str) = target.object.to_str() {
                    obj_str_list.push(obj_str.to_owned());
                }
            }
        }
        Ok(obj_str_list.join(" "))
    }

    // This might become a pain point when we decided to support mixed language
    // projects
    pub fn find_source_files(&mut self) -> Result<(), YabsError> {
        // If sources are listed don't walk the current directory for files
        if self.src.is_some() {
            for entry in self.src.clone().unwrap() {
                let modified_time = metadata(&entry)?.modified()?;
                if let Some(src_str) = entry.clone().to_str() {
                    &self.file_mod_map
                         .insert(Target::new(entry,
                                             PathBuf::from(src_str.replace(&self.lang, "o"))),
                                 modified_time);
                }
            }
        } else {
            &self.walk_current_dir()?;
        }
        Ok(())
    }

    fn walk_current_dir(&mut self) -> Result<(), YabsError> {
        let mut sources = Vec::new();
        let mut file_mod_map = BTreeMap::new();
        let walk_dir = WalkDir::new(".").into_iter();
        for entry in walk_dir.filter_entry(|e| !&self.is_in_ignore(e)) {
            let entry = entry?;
            if entry.path().is_file() {
                let file_ext = entry.path().extension().unwrap_or(OsStr::new(""));
                if let Some(ext) = file_ext.to_str() {
                    if ext == &self.lang {
                        if let Some(src_str) = entry.path().to_str() {
                            let modified_time = metadata(entry.path())?.modified()?;
                            file_mod_map
                                .insert(Target::new(entry.path().to_path_buf(),
                                                    PathBuf::from(src_str.replace(&self.lang,
                                                                                  "o"))),
                                        modified_time);
                        }
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
