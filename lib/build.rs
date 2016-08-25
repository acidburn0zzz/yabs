// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;
extern crate rustc_serialize;
extern crate walkdir;
extern crate rpf;

use error::YabsError;
use rpf::*;
use ext::*;
use desc::*;

use std::fs::File;
use std::io::{Write};

// Profile has descriptions that describe build instructions (proj_desc),
// install instructions (inst_desc), and documentation instructions (doc_desc).
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct Profile {
    name: String,
    proj_desc: Option<ProjDesc>,
    inst_desc: Option<InstallDesc>,
    doc_desc: Option<DocDesc>,
}

// A build file could have multiple `Profile`s
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone)]
pub struct BuildFile {
    profiles: Vec<Profile>,
}

impl BuildFile {
    // TODO: Find a better way to do this
    pub fn apply_all(&mut self) {
        let mut all: Profile = Default::default();
        for profile in &self.profiles {
            if profile.name == "all" {
                all = profile.clone();
            }
        }
        for profile in &mut self.profiles {
            if let Some(proj_desc) = all.proj_desc.clone() {
                profile.proj_desc = Some(proj_desc);
            }
            if let Some(inst_desc) = all.inst_desc.clone() {
                profile.inst_desc = Some(inst_desc);
            }
            if let Some(doc_desc) = all.doc_desc.clone() {
                profile.doc_desc = Some(doc_desc);
            }
        }
    }

    // Creates a `Profiles` from a toml file.
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

    pub fn print_as_json(&mut self) {
        self.apply_all();
        for profile in &self.profiles {
            profile.print_json();
        }
    }

    pub fn print_available_profiles(&mut self) {
        self.apply_all();
        for profile in &self.profiles {
            print!("{} ", profile.name);
        }
        print!("\n");
    }

    // Prints a profile with name `name` in build file as JSON
    pub fn print_profile_as_json(&mut self, name: String) {
        self.apply_all();
        for profile in &self.profiles {
            if profile.name == name {
                profile.print_json();
            }
        }
    }

    // Generate a Makefile using from a profile with name `name`
    pub fn gen_make(&mut self, name: String) -> Result<(), YabsError> {
        self.apply_all();
        if let Some(profile) = self.profiles.iter().find(|ref profile| profile.name == name) {
            let mut file = try!(File::create("Makefile"));
            if let Some(mut proj_desc) = profile.proj_desc.clone() {
                try!(file.write(try!(proj_desc.gen_make()).as_bytes()));
            }
            if let Some(doc_desc) = profile.doc_desc.clone() {
                try!(file.write(doc_desc.gen_make().as_bytes()));
            }
            Ok(())
        } else {
            Err(YabsError::NoDesc(name))
        }
    }

    pub fn build(&mut self, name: String) -> Result<(), YabsError> {
        self.apply_all();
        if let Some(profile) = self.profiles.iter().find(|ref profile| profile.name == name) {
            try!(profile.clone().proj_desc.unwrap().build_bin());
            Ok(())
        } else {
            Err(YabsError::NoDesc(name))
        }
    }

    pub fn print_sources(&mut self) -> Result<(), YabsError> {
        for profile in &mut self.profiles {
            if let &mut Some(ref mut proj) = &mut profile.proj_desc {
                println!("{}", profile.name.bold());
                try!(proj.gen_file_list());
                if let Some(set_sources) = proj.src.as_ref() {
                    for src in set_sources {
                        println!("{}", src);
                    }
                }
            }
        }
        Ok(())
    }
}

// Descibe how to install this project
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct InstallDesc {
    prefix: Option<String>,
}

// Generate documentation for this project
#[derive(Debug,Default,RustcDecodable,RustcEncodable,Clone,PartialEq)]
pub struct DocDesc {
    doc: Option<Vec<String>>,
}

impl DocDesc {
    pub fn gen_make(&self) -> String {
        let mut doc_str = String::from("doc:\n");
        if let Some(doc) = self.doc.clone() {
            for line in doc {
                doc_str.push_str(&format!("\t{}\n", &line));
            }
        }
        return doc_str;
    }
}

#[test]
fn test_empty_buildfile() {
    let bf = BuildFile::from_file("test/empty.toml").unwrap();
    assert_eq!(bf.profiles.len(), 0);
}

#[test]
#[should_panic]
fn test_non_empty_buildfile() {
    let bf = BuildFile::from_file("test/test_project/test.toml").unwrap();
    assert_eq!(bf.profiles.len(), 0);
}

#[test]
fn test_buildfile_gen_make() {
    let mut bf = BuildFile::from_file("test/test_project/test.toml").unwrap();
    assert_eq!(bf.gen_make("linux_cpp".to_owned()).unwrap(), ());
}

#[test]
fn test_buildfile_build() {
    let mut bf = BuildFile::from_file("test/test_project/test.toml").unwrap();
    assert_eq!(bf.build("linux_cpp".to_owned()).unwrap(), ());
}
