extern crate toml;
extern crate serde;
extern crate serde_json;
extern crate walkdir;

use error::YabsError;
use ext::*;
use serde::ser::Serialize;
use serde::de::Deserialize;

pub struct Target {
    pub target: String,
    pub all: String,
}

// General trait for any description.
pub trait Desc<T> {
    fn new() -> T;
    fn from_toml_table(table: toml::Value) -> Result<T, YabsError>;
    fn print_json(&self) -> Result<(), YabsError>;
}

impl<T: Serialize + Deserialize + Default> Desc<T> for T {
    // Creates an empty description using `Default`
    fn new() -> T {
        Default::default()
    }

    fn from_toml_table(table: toml::Value) -> Result<T, YabsError> {
        Ok(Deserialize::deserialize(&mut toml::Decoder::new(table.clone()))?)
    }

    fn print_json(&self) -> Result<(), YabsError> {
        Ok(println!("{}", serde_json::to_string_pretty(&self)?))
    }
}

pub trait FromFile<T> {
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>>;
}

// FromFile for anything T that implements Desc etc.
impl<T: Serialize + Deserialize + Default + Desc<T>> FromFile<T> for T {
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>> {
        parse_toml_file(file).and_then(|toml| {
            toml.get(name)
                .ok_or(vec![YabsError::NoDesc(name.to_owned())])
                .and_then(|desc| {
                    Deserialize::deserialize(&mut toml::Decoder::new(desc.clone()))
                        .map_err(|err| vec![YabsError::TomlDecode(err)])
                })
        })
    }
}
