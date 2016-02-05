extern crate toml;

use std::fs::File;
use std::path::Path;
use std::io::prelude::*;
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
