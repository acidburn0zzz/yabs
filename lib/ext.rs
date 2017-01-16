// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;

use std::fs::File;
use std::path::Path;
use std::io;
use std::io::Read;
use std::env;
use std::process::Command;
use error::YabsError;

pub fn parse_toml_file<T: AsRef<Path> + Clone>(file: T) -> Result<toml::Table, Vec<YabsError>> {
    let mut buff = String::new();
    let mut error_vect = Vec::new();
    let mut file = match File::open(&file) {
        Ok(s) => s,
        Err(e) => {
            match e.kind() {
                io::ErrorKind::NotFound => {
                    error_vect.push(YabsError::NoAssumedToml(file.as_ref().to_string_lossy().into_owned()));
                },
                _ => {
                    error_vect.push(YabsError::Io(e));
                },
            };
            return Err(error_vect);
        }
    };
    if let Err(err) = file.read_to_string(&mut buff) {
        error_vect.push(YabsError::Io(err));
    }
    let mut parser = toml::Parser::new(&buff);
    if let Some(table) = parser.parse() {
        return Ok(table);
    } else {
        for err in parser.errors {
            error_vect.push(YabsError::TomlParse(err));
        }
    }
    return Err(error_vect);
}

pub fn get_assumed_filename() -> Option<String> {
    if let Ok(current_dir) = env::current_dir() {
        if let Some(file_stem) = current_dir.components().last() {
            let mut file_name = file_stem.as_ref().to_string_lossy().into_owned();
            file_name.push_str(".toml");
            return Some(file_name);
        }
    }
    None
}

pub fn run_cmd(cmd: &String) -> Result<(), YabsError> {
    let command = Command::new("sh")
                           .arg("-c")
                           .arg(&cmd)
                           .spawn()?
                           .wait_with_output()?;
    println!("{}", &cmd);
    if !command.status.success() {
        print!("{}", String::from_utf8(command.stderr)?);
        return Err(YabsError::Command(cmd.to_owned(), command.status.code().unwrap_or(1)));
    }
    print!("{}", String::from_utf8(command.stdout)?);
    Ok(())
}
