// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate regex;

use std::old_path::GenericPath;
use std::old_io::fs;
use std::old_io::fs::{PathExtensions};

#[derive(Default)]
pub struct Gen {
    pub file_list: Vec<Path>,
}

impl Gen {
    pub fn recur_walk(&mut self, dir: &Path) {
        let contents = fs::readdir(dir);
        for items in contents.unwrap().iter() {
            if items.is_file() {
                self.file_list.push(items.clone());
            } else if items.is_dir() {
                self.recur_walk(items);
            }
        }
    }
    
    pub fn print_file_list(&mut self) {
        for i in self.file_list.iter() {
            println!("{}", i.display());
        }
    }
}
