// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

use std::path::{PathBuf};
use std::fs::{read_dir,PathExt};

#[derive(Default)]
pub struct Gen {
    pub file_list: Vec<PathBuf>,
}

impl Gen {
    pub fn is_dot(&mut self, dir: &PathBuf) -> bool {
        let file_string = String::from_str(dir.file_name().unwrap().to_str().unwrap());
        if file_string.starts_with(".") {
            return true;
        } else {
            return false;
        }
    }

    pub fn has_ext(&mut self, dir: &PathBuf, ext: &String) -> bool {
        if dir.extension().is_some() {
            if &String::from_str(dir.extension().unwrap().to_str().unwrap()) == ext {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    pub fn recur_walk(&mut self, dir: &PathBuf, ext: &String) {
        let contents = read_dir(dir);
        for items in contents.unwrap() {
            let item_path = items.unwrap().path();
            if item_path.is_file() {
                if !self.is_dot(&item_path) {
                    if self.has_ext(&item_path, ext) {
                        self.file_list.push(item_path);
                    }
                }
            } else {
                if !self.is_dot(&item_path) {
                    self.recur_walk(&item_path, ext);
                }
            }
        }
    }

    pub fn print_file_list(&mut self) {
        for i in self.file_list.iter() {
            println!("{}", i.display());
        }
    }
}
