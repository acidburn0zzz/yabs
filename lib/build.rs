// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate serde;
extern crate toml;
extern crate walkdir;
extern crate ansi_term;

use desc::project::*;
use error::{YabsError, YabsErrorKind};
use ext::{Job, PrependEach, get_assumed_filename_for_dir, run_cmd, spawn_cmd};

use std::collections::BTreeSet;
use std::env;
use std::fs;
use std::fs::File;
use std::io::prelude::*;
use std::path::{Path, PathBuf};
use std::process::Child;

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

    fn spawn_build_object(&self, target: &Target) -> Result<(String, Child), YabsError> {
        let command = &format!("{CC} -c {CFLAGS} {INC} -o {OBJ} {SRC}",
                CC =
                    &self.project.compiler.as_ref().unwrap_or(&String::from("gcc")),
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
                SRC = target.source().to_str().unwrap());
        Ok((command.to_owned(), spawn_cmd(&command)?))
    }

    fn build_all_binaries(&mut self, jobs: usize) -> Result<(), YabsError> {
        let mut queue = BTreeSet::new();
        if !&self.binaries.is_some() {
            return Ok(());
        }
        for binary in self.binaries.clone().unwrap().iter() {
            if Path::new(&binary.name()).exists() {
                for (target, modtime) in &self.project.file_mod_map {
                    if modtime > &fs::metadata(&binary.name())?.modified()? ||
                       !target.object().exists() {
                        queue.insert(target.clone());
                    }
                }
            } else {
                for (target, _) in &self.project.file_mod_map {
                    if !target.object().exists() {
                        queue.insert(target.clone());
                    }
                }
            }
        }

        &self.run_job_queue(queue.iter().cloned().collect(), jobs)?;
        for binary in self.binaries.clone().unwrap().iter() {
            &self.build_binary(binary)?;
        }
        Ok(())
    }

    fn run_job_queue(&mut self, mut job_queue: Vec<Target>, jobs: usize) -> Result<(), YabsError> {
        let mut job_processes: Vec<Job> = Vec::new();
        while !job_queue.is_empty() {
            if job_processes.len() < jobs {
                if let Some(target) = job_queue.pop() {
                    let job = Job::new(self.spawn_build_object(&target)?);
                    info!("{}", job.command());
                    job_processes.push(job);
                }
            } else {
                while !job_processes.is_empty() {
                    if let Some(mut job) = job_processes.pop() {
                        job.yield_self()?;
                    }
                }
            }
        }
        while !job_processes.is_empty() {
            if let Some(mut job) = job_processes.pop() {
                job.yield_self()?;
            }
        }
        Ok(())
    }

    fn build_binary(&self, binary: &Binary) -> Result<(), YabsError> {
        let object_list;
        if self.binaries.as_ref().unwrap().len() == 1 {
            object_list = self.project.object_list_as_string(None)?;
        } else {
            object_list =
                self.project
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
                           CC = &self.project.compiler.as_ref().unwrap_or(&String::from("gcc")),
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
                            ARFLAGS =
                                &self.project.arflags.as_ref().unwrap_or(&String::from("rcs")),
                            LIB = library.path().to_str().unwrap(),
                            OBJ_LIST = object_list))?
        }
        Ok(())
    }

    pub fn build(&mut self, jobs: usize) -> Result<(), YabsError> {
        &self.project.run_script(&self.project.before_script)?;
        &self.build_all_binaries(jobs)?;
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
