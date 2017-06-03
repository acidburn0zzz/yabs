// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate serde;
extern crate toml;
extern crate walkdir;
extern crate ansi_term;

use desc::project::*;
use error::{YabsError, YabsErrorKind};
use ext::{PrependEach, run_cmd, get_assumed_filename_for_dir};
use std::fs;

use std::fs::File;
use std::env;
use std::io::prelude::*;
use std::path::{Path, PathBuf};

// A build file could have multiple `Profile`s
#[derive(Debug, Default, Clone, Serialize, Deserialize)]
pub struct BuildFile {
    project: ProjectDesc,
    #[serde(rename = "bin")]
    binaries: Option<Vec<Binary>>,
    #[serde(rename = "lib")]
    libraries: Option<Vec<Library>>,
}

impl BuildFile {
    // Creates a `Profiles` from a toml file. Is essentiall `BuildFile::new`
    pub fn from_file<T: AsRef<Path>>(filepath: &T) -> Result<BuildFile, YabsError> {
        let mut buffer = String::new();
        let mut file = File::open(filepath)?;
        file.read_to_string(&mut buffer)?;
        let mut build_file: BuildFile = toml::from_str(&buffer)?;
        build_file.project.find_source_files()?;
        Ok(build_file)
    }

    pub fn print_sources(&mut self) {
        for target in self.project.file_mod_map.keys() {
            info!("{}", target.source().display());
        }
    }

    fn build_object(&self, target: &Target) -> Result<(), YabsError> {
        Ok(run_cmd(&format!("{CC} -c {CFLAGS} {INC} -o {OBJ} {SRC}",
                           CC = &self.project
                                     .compiler
                                     .as_ref()
                                     .unwrap_or(&String::from("gcc")),
                           CFLAGS = &self.project
                                         .compiler_flags
                                         .as_ref()
                                         .unwrap_or(&vec![])
                                         .prepend_each("-")
                                         .join(" "),
                           INC = &self.project
                                      .include
                                      .as_ref()
                                      .unwrap_or(&vec![])
                                      .prepend_each("-I")
                                      .join(" "),
                           OBJ = target.object().to_str().unwrap(),
                           SRC = target.source().to_str().unwrap()))?)
    }

    fn build_all_binaries(&mut self) -> Result<(), YabsError> {
        if !&self.binaries.is_some() {
            return Ok(());
        }
        for binary in self.binaries.clone().unwrap().iter() {
            if Path::new(&binary.name()).exists() {
                for (target, modtime) in &self.project.file_mod_map {
                    if modtime > &fs::metadata(&binary.name())?.modified()? {
                        &self.build_object(target)?;
                    }
                }
                &self.build_binary(binary)?;
            } else {
                for target in self.project.file_mod_map.keys() {
                    if !target.object().exists() {
                        &self.build_object(target)?;
                    }
                }
                &self.build_binary(binary)?;
            }
        }
        Ok(())
    }

    fn build_binary(&self, binary: &Binary) -> Result<(), YabsError> {
        let object_list;
        if self.binaries.as_ref().unwrap().len() == 1 {
            object_list = self.project.object_list_as_string(None)?;
        } else {
            object_list = self.project
                .object_list_as_string(Some(self.binaries
                                                .clone()
                                                .unwrap()
                                                .into_iter()
                                                .filter(|ref bin| {
                                                            bin.path() != binary.path()
                                                        })
                                                .collect::<Vec<Binary>>()))?;
        }
        Ok(run_cmd(&format!("{CC} {LFLAGS} -o {BIN} {OBJ_LIST} {LIB_DIR} {LIBS}",
                           CC = &self.project
                                     .compiler
                                     .as_ref()
                                     .unwrap_or(&String::from("gcc")),
                           LFLAGS = &self.project
                                         .lflags
                                         .as_ref()
                                         .unwrap_or(&vec![])
                                         .prepend_each("-")
                                         .join(" "),
                           BIN = binary.name(),
                           OBJ_LIST = object_list,
                           LIB_DIR = &self.project
                                          .lib_dir
                                          .as_ref()
                                          .unwrap_or(&vec![])
                                          .prepend_each("-L")
                                          .join(" "),
                           LIBS = &self.project.libs_as_string()))?)
    }

    fn build_library(&mut self) -> Result<(), YabsError> {
        if !self.libraries.is_some() {
            return Ok(());
        }
        for library in self.libraries.clone().unwrap().iter() {
            let object_list = &self.project.object_list_as_string(None)?;
            run_cmd(&format!("{AR} {ARFLAGS} {LIB} {OBJ_LIST}",
                            AR = &self.project.ar.as_ref().unwrap_or(&String::from("ar")),
                            ARFLAGS = &self.project
                                           .arflags
                                           .as_ref()
                                           .unwrap_or(&String::from("rcs")),
                            LIB = library.path().to_str().unwrap(),
                            OBJ_LIST = object_list))?
        }
        Ok(())
    }

    pub fn build(&mut self) -> Result<(), YabsError> {
        &self.project.run_script(&self.project.before_script)?;
        &self.build_all_binaries()?;
        &self.build_library()?;
        &self.project.run_script(&self.project.after_script)?;
        Ok(())
    }

    pub fn clean(&self) -> Result<(), YabsError> {
        for target in self.project.file_mod_map.keys() {
            if target.object().exists() {
                if let Ok(_) = fs::remove_file(target.object()) {
                    info!("removed object '{}'", target.object().display());
                }
            }
        }
        if let Some(binaries) = self.binaries.clone() {
            for binary in binaries {
                let bin_path = PathBuf::from(binary.name());
                if bin_path.exists() {
                    if let Ok(_) = fs::remove_file(&bin_path) {
                        info!("removed binary '{}'", bin_path.display());
                    }
                }
            }
        }
        if let Some(libraries) = self.libraries.clone() {
            for library in libraries {
                if library.path().exists() {
                    if let Ok(_) = fs::remove_file(library.path()) {
                        info!("removed library '{}'", library.path().display());
                    }
                }
            }
        }
        Ok(())
    }
}

pub fn find_build_file(dir: &mut PathBuf) -> Result<BuildFile, YabsError> {
    loop {
        if let Some(filepath) = check_dir(&dir) {
            env::set_current_dir(&dir)?;
            return Ok(BuildFile::from_file(&dir.join(filepath))?);
        } else {
            if !dir.pop() {
                break;
            }
        }
    }
    bail!(YabsErrorKind::NoAssumedToml(dir.to_str().unwrap().to_owned()))
}

fn check_dir(dir: &PathBuf) -> Option<PathBuf> {
    if let Some(assumed) = get_assumed_filename_for_dir(dir) {
        if dir.join(&assumed).exists() {
            return Some(dir.join(assumed));
        }
    }
    None
}

#[test]
#[should_panic]
fn test_empty_buildfile() {
    let bf = BuildFile::from_file(&"test/empty.toml").unwrap();
    assert_eq!(bf.binaries.unwrap().len(), 0);
}

#[test]
#[should_panic]
fn test_non_empty_buildfile() {
    let bf = BuildFile::from_file(&"test/test_project/test.toml").unwrap();
    let default_proj: ProjectDesc = Default::default();
    assert_eq!(bf.project, default_proj);
}
