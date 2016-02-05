extern crate toml;
extern crate rustc_serialize;

use toml::decode;
use error::YabsError;
use ext::*;
use rustc_serialize::{Decodable, Decoder, Encodable, Encoder};

pub struct BuildFile {
    proj_desc: Option<ProjDesc>,
    install_dsec: Option<InstallDesc>,
}

#[derive(Debug,Default,RustcDecodable)]
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

#[derive(Debug,Default,RustcDecodable)]
pub struct InstallDesc {
    prefix: Option<String>,
}

pub trait Desc<T> {
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>>;
}

impl <T: Decodable> Desc<T> for T {
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
}
