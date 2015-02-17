extern crate regex;

use std::old_path::GenericPath;
use std::old_io::fs;
use std::old_io::fs::{PathExtensions};

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
