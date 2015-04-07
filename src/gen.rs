// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

use std::env;
use std::path::Path;
use std::path::{PathBuf};
use std::fs::{read_dir,PathExt};
use std::ffi::AsOsStr;

#[derive(Default)]
pub struct Gen {
    pub file_list: Vec<PathBuf>,
}

impl Gen {
    pub fn is_dot(&self, dir: &PathBuf) -> bool {
        if dir.starts_with(".") {
            return true;
        } else {
            return false;
        }
    }

    pub fn has_ext(&self, dir: &PathBuf, ext: &String) -> bool {
        match dir.extension() {
            Some(x) => {
                if ext.as_os_str() == x {
                    return true;
                }
            }
            None => return false
        };
        return false;
    }

    pub fn walk_dir(&mut self, dir: PathBuf, ext: &String) {
        if dir.is_dir() {
            for cont in read_dir(&dir).unwrap() {
                match cont {
                    Ok(entry) => {
                        if !self.is_dot(&entry.path()) &&
                            self.has_ext(&entry.path(), &ext) {
                                self.file_list.push(
                                    (entry.path().relative_from(
                                            &env::current_dir().unwrap()
                                            ).unwrap()).to_path_buf()
                                    );
                            }
                        if entry.path().is_dir() {
                            self.walk_dir(entry.path(), &ext);
                        }
                    }
                    Err(e) => {
                        panic!(e.to_string());
                    }
                };
            }
        }
    }

    pub fn print_filelist(self) {
        for i in self.file_list.iter() {
            println!("{}", i.display());
        }
    }
}
