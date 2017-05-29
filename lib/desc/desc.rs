extern crate toml;
extern crate serde;
extern crate serde_json;
extern crate walkdir;

use error::YabsError;
use serde::de::Deserialize;
use serde::ser::Serialize;

use std::fs::File;
use std::io::prelude::*;

pub trait FromFile<T> {
    fn from_file(file: &str) -> Result<T, YabsError>;
}

// FromFile for anything T that implements Desc etc.
impl<T: Serialize + Deserialize> FromFile<T> for T {
    fn from_file(file: &str) -> Result<T, YabsError> {
        let mut buffer = String::new();
        let mut file = File::open(file)?;
        file.read_to_string(&mut buffer)?;
        Ok(toml::from_str(&buffer)?)
    }
}
