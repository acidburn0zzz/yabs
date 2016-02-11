// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;

use std::fs::File;
use std::path::Path;
use std::io::prelude::*;
use std::env;
use error::YabsError;

pub fn parse_toml_file<T: AsRef<Path>>(file: T) -> Result<toml::Table, Vec<YabsError>> {
    let mut buff = String::new();
    let mut error_vect = Vec::new();
    let mut file = match File::open(file) {
        Ok(s) => s,
        Err(e) => {
            error_vect.push(YabsError::Io(e));
            return Err(error_vect);
        }
    };
    match file.read_to_string(&mut buff) {
        Ok(s) => s,
        Err(e) => {
            error_vect.push(YabsError::Io(e));
            return Err(error_vect);
        }
    };
    let mut parser = toml::Parser::new(&buff);
    match parser.parse() {
        Some(s) => return Ok(s),
        None => {
            for err in parser.errors {
                error_vect.push(YabsError::TomlParse(err));
            }
            return Err(error_vect);
        }
    };
}

pub fn get_assumed_filename() -> Option<String> {
    if let Ok(current_dir) = env::current_dir() {
        if let Some(file_stem) = current_dir.components().last() {
            let mut file_name: String = String::from(file_stem.as_ref().to_str().unwrap_or(""));
            file_name.push_str(".toml");
            return Some(file_name);
        }
    }
    None
}
