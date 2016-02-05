extern crate toml;
extern crate rustc_serialize;

use toml::decode;
use error::YabsError;
use ext::*;
use rustc_serialize::{Decodable, Decoder, Encodable, Encoder, json};

#[derive(Debug,Default,RustcDecodable,RustcEncodable)]
pub struct BuildFile {
    proj_desc: Option<ProjDesc>,
    install_desc: Option<InstallDesc>,
}

impl BuildFile {
    fn from_file(file: &str) -> Result<BuildFile, Vec<YabsError>> {
        let mut build_file: BuildFile = Default::default();
        parse_toml_file(file)
            .and_then(|toml| {
                for (key, table) in toml {
                    match key.as_ref() {
                        "project" => build_file.proj_desc = ProjDesc::from_toml_table(table).ok(),
                        "install" => build_file.install_desc = InstallDesc::from_toml_table(table).ok(),
                        _ => (),
                    }
                }
                Ok(build_file)
            })
            .map_err(|err| err)
    }
    fn print_as_json(self) {
        println!("[project]");
        self.proj_desc.unwrap_or(ProjDesc::new()).print_json();
    }
}

#[derive(Debug,Default,RustcDecodable,RustcEncodable)]
pub struct ProjDesc {
    name: Option<String>,
    target: Option<String>,
    os: Option<String>,
    version: Option<String>,
    compiler: Option<String>,
    libs: Option<Vec<String>>,
    inc: Option<Vec<String>>,
    cflags: Option<String>,
    lflags: Option<String>,
    ignore: Option<Vec<String>>,
    before_script: Option<Vec<String>>,
    after_script: Option<Vec<String>>,
    lib: Option<bool>,
}

impl ProjDesc {
    pub fn new() -> ProjDesc {
        Default::default()
    }
}

// Descibe how to install this project
#[derive(Debug,Default,RustcDecodable,RustcEncodable)]
pub struct InstallDesc {
    prefix: Option<String>,
}

// General trait for any descriptions
pub trait Desc<T> {
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>>;
    fn from_toml_table(table: toml::Value) -> Result<T, YabsError>;
    fn print_json(&self);
}

impl <T: Decodable + Encodable> Desc<T> for T {
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>> {
        parse_toml_file(file).and_then(|toml| {
            toml.get(name)
                .ok_or(vec![YabsError::NoDesc(name.to_owned())])
                .and_then(|desc| {
                    Decodable::decode(&mut toml::Decoder::new(desc.clone()))
                        .map_err(|err| vec![YabsError::TomlDecode(err)])
                })
        })
    }

    fn from_toml_table(table: toml::Value) -> Result<T, YabsError> {
        Ok(try!(Decodable::decode(&mut toml::Decoder::new(table))))
    }

    fn print_json(&self) {
        println!("{}", json::as_pretty_json(&self));
    }
}
