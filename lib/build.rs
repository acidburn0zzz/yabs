extern crate toml;
extern crate rustc_serialize;
extern crate walkdir;
extern crate rpf;

use toml::decode;
use error::YabsError;
use ext::*;
use rustc_serialize::{Decodable, Decoder, Encodable, Encoder, json};
use walkdir::WalkDir;

use std::ffi::OsStr;
use std::path::PathBuf;

#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct Profile {
    name: String,
    proj_desc: Option<ProjDesc>,
    inst_desc: Option<InstallDesc>,
    doc_desc: Option<DocDesc>,
}

#[derive(Debug,Default,RustcDecodable,RustcEncodable)]
pub struct BuildFile {
    profiles: Vec<Profile>,
}

impl BuildFile {
    pub fn from_file(file: &str) -> Result<BuildFile, Vec<YabsError>> {
        let mut build_file: BuildFile = Default::default();
        parse_toml_file(file)
            .and_then(|toml| {
                for (key, table) in toml {
                    let mut profile: Profile = Default::default();
                    profile.name = key.clone();
                    match table {
                        toml::Value::Table(inner_table) => {
                            for (key, table) in inner_table {
                                match key.as_ref() {
                                    "project" => {
                                        profile.proj_desc = ProjDesc::from_toml_table(table).ok()
                                    }
                                    "install" => {
                                        profile.inst_desc = InstallDesc::from_toml_table(table).ok()
                                    }
                                    "doc" => {
                                        profile.doc_desc = DocDesc::from_toml_table(table).ok()
                                    }
                                    _ => (),
                                }
                            }
                        }
                        _ => (),
                    };
                    build_file.profiles.push(profile);
                }
                Ok(build_file)
            })
            .map_err(|err| err)
    }

    pub fn print_as_json(&self) {
        for profile in &self.profiles {
            profile.print_json();
        }
    }

    pub fn print_available_profiles(&self) {
        for profile in &self.profiles {
            print!("{} ", profile.name);
        }
        print!("\n");
    }

    pub fn print_profile_as_json(&self, name: String) {
        for profile in &self.profiles {
            if profile.name == name {
                profile.print_json();
            }
        }
    }

    // Hideous Makefile generation to be implemented
    //pub fn gen_make(&self, name: String) -> Result<(), YabsError> {
        //for profile in &self.profiles {
            //if profile.name == name {
                //continue;
            //} else {
                //return Ok(());
            //}
        //}
        //let format = format!("INSTALL\t=\t/usr/bin/env \
                              //install\nDEST\t=\nPREFIX\t=\nCC\t=\t{}\nBINDIR\t=\nLIBDIR\t=\nTARGE\
                              //T\t=\nLINK\t=\nCFLAGS\t=\nLFLAGS\t=\nLIBS\t=\nINCDIR\t=\nLIBDIR\t=\
                              //\nCLEAN\t=\n\nDEL\t=\trm -f\n\n.PHONY: doc \
                              //clean\n\n.cpp.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \
                              //\"$<\"\n.cc.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \
                              //\"$<\"\n\n.cxx.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \
                              //\"$<\"\n\n.C.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \
                              //\"$<\"\n\n.c.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \
                              //\"$<\"\n\nall: $(TRGT)\n",
                             //&self.profiles[1].clone().proj_desc.unwrap().compiler.unwrap());
        //print!("{}", format);
        //Ok(())
    //}

    pub fn print_sources(&self) -> Result<(), YabsError> {
        for profile in &self.profiles {
            println!("{}", profile.name);
            if let Some(proj) = profile.proj_desc.as_ref() {
                let sources = try!(proj.gen_file_list());
                for file in sources.files {
                    println!("{}", file.display());
                }
            }
        }
        Ok(())
    }
}

#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct ProjDesc {
    name: Option<String>,
    target: Option<String>,
    lang: Option<String>,
    os: Option<String>,
    version: Option<String>,
    compiler: Option<String>,
    src: Option<Vec<String>>,
    libs: Option<Vec<String>>,
    lib_dir: Option<Vec<String>>,
    inc: Option<Vec<String>>,
    inc_dir: Option<Vec<String>>,
    cflags: Option<String>,
    lflags: Option<String>,
    ignore: Option<Vec<String>>,
    before_script: Option<Vec<String>>,
    after_script: Option<Vec<String>>,
    lib: Option<bool>,
    ar: Option<String>,
}


#[derive(Default,RustcDecodable,RustcEncodable,PartialEq)]
struct Sources {
    files: Vec<PathBuf>,
    objects: Vec<PathBuf>,
}

impl ProjDesc {
    fn gen_file_list(&self) -> Result<Sources, YabsError> {
        let mut sources = Sources::new();
        for entry in WalkDir::new(".") {
            let entry = try!(entry);
            if entry.path().is_file() {
                let lang = &self.lang.clone().unwrap();
                let file_ext = entry.path().extension().unwrap_or(OsStr::new(""));
                if let Some(ext) = file_ext.to_str() {
                    if let Some(lang) = self.lang.clone() {
                        if ext == lang {
                            sources.files.push(PathBuf::from(entry.path()));
                        }
                    }
                }
            }
        }
        Ok(sources)
    }
}

// Descibe how to install this project
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct InstallDesc {
    prefix: Option<String>,
}

#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct DocDesc {
    doc: Option<String>,
}

// General trait for any descriptions
pub trait Desc<T> {
    fn new() -> T;
    fn from_file(file: &str, name: &str) -> Result<T, Vec<YabsError>>;
    fn from_toml_table(table: toml::Value) -> Result<T, YabsError>;
    fn print_json(&self);
}

impl<T: Decodable + Encodable + Default> Desc<T> for T {
    fn new() -> T {
        Default::default()
    }

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
        Ok(try!(Decodable::decode(&mut toml::Decoder::new(table.clone()))))
    }

    fn print_json(&self) {
        println!("{}", json::as_pretty_json(&self));
    }
}
