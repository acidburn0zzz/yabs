// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;
extern crate rustc_serialize;
extern crate walkdir;
extern crate rpf;

use toml::decode;
use error::YabsError;
use rpf::*;
use ext::*;
use rustc_serialize::{Decodable, Decoder, Encodable, Encoder, json};
use walkdir::WalkDir;

use std::ffi::OsStr;
use std::fs::File;
use std::io::Write;

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

    pub fn gen_make(&self, name: String) -> Result<(), YabsError> {
        if let Some(index) = self.profiles.iter().position(|ref profile| profile.name == name) {
            try!(try!(File::create("Makefile")).write_all(self.profiles[index.clone()]
                                                              .clone()
                                                              .proj_desc
                                                              .unwrap_or(ProjDesc::new())
                                                              .gen_make()
                                                              .as_bytes()));
            Ok(())
        } else {
            Err(YabsError::NoDesc(name))
        }
    }

    pub fn print_sources(&self) -> Result<(), YabsError> {
        for profile in &self.profiles {
            if let Some(proj) = profile.proj_desc.as_ref() {
                println!("{}", profile.name.bold());
                let sources = try!(proj.clone().gen_file_list());
                for file in sources.files {
                    println!("{}", file);
                }
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
    cflags: Option<Vec<String>>,
    explicit_cflags: Option<String>,
    lflags: Option<String>,
    ignore: Option<Vec<String>>,
    before_script: Option<Vec<String>>,
    after_script: Option<Vec<String>>,
    lib: Option<bool>,
    ar: Option<String>,
    clean: Option<Vec<String>>,
}

#[derive(Default,RustcDecodable,RustcEncodable,PartialEq)]
struct Sources {
    files: Vec<String>,
    objects: Vec<String>,
}

impl ProjDesc {
    fn concat_clean(&self) -> String {
        self.prepend_op_vec(&self.clean, "".to_string())
    }

    fn gen_file_list(&self) -> Result<Sources, YabsError> {
        let mut sources = Sources::new();
        for entry in WalkDir::new(".") {
            let entry = try!(entry);
            if entry.path().is_file() {
                let file_ext = entry.path().extension().unwrap_or(OsStr::new(""));
                if let Some(ext) = file_ext.to_str() {
                    if let Some(lang) = self.lang.clone() {
                        if ext == lang {
                            sources.files.push(entry.path().as_string());
                        }
                    }
                }
            }
        }
        Ok(sources)
    }


    fn prepend_op_vec(&self, list: &Option<Vec<String>>, prepend: String) -> String {
        let mut horrid_string = String::new();
        if let Some(items) = list.as_ref() {
            if let Some(split_last) = items.split_last() {
                for sub_item in split_last.1 {
                    horrid_string.push_str(&format!("{}{} ", prepend, sub_item));
                }
                horrid_string.push_str(&format!("{}{}", prepend, split_last.0.clone()));
            }
        }
        return horrid_string;
    }

    fn gen_make_lib_list(&self) -> String {
        return self.prepend_op_vec(&self.libs, "-l".to_string());
    }

    fn gen_make_cflags_list(&self) -> String {
        return self.prepend_op_vec(&self.cflags, "-".to_string());
    }

    fn gen_make_src_list(&self) -> String {
        let mut horrid_string: String = "SRC\t= ".to_owned();
        let mut lang = self.lang.clone().unwrap_or("cpp".to_owned());
        lang.insert(0, '.');
        if let Some(source_list) = self.src.as_ref() {
            if let Some(split_first) = source_list.split_first() {
                horrid_string.push_str(&format!("{} \\\n", split_first.0));
                if let Some(split_last) = split_first.1.clone().split_last() {
                    for src in split_last.1 {
                        horrid_string.push_str(&format!("\t{} \\\n", src));
                    }
                    horrid_string.push_str(&format!("\t{}\n", split_last.0));
                }
            }
            let mut parsed_obj_list = Vec::new();
            for obj in source_list {
                parsed_obj_list.push(obj.replace(&lang, ".o"));
            }
            horrid_string.push_str("OBJ\t= ");
            if let Some(split_first) = parsed_obj_list.split_first() {
                horrid_string.push_str(&format!("{} \\\n", split_first.0));
                if let Some(split_last) = split_first.1.clone().split_last() {
                    for src in split_last.1 {
                        horrid_string.push_str(&format!("\t{} \\\n", src));
                    }
                    horrid_string.push_str(&format!("\t{}", split_last.0));
                }
            }
        }
        return horrid_string;
    }

    fn gen_make_src_deps(&self) -> String {
        let mut horrid_string = String::new();
        let mut lang = self.lang.clone().unwrap_or("cpp".to_owned());
        lang.insert(0, '.');
        if let Some(source_list) = self.src.as_ref() {
            for src in source_list {
                horrid_string.push_str(&format!("{0}: {1}\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \
                                                 {0} {1}\n\n",
                                                src.replace(&lang, ".o"),
                                                src));
            }
        }
        return horrid_string;
    }

    fn gen_make_inc_list(&self) -> String {
        return self.prepend_op_vec(&self.inc, "-I".to_string());
    }

    fn gen_make(&self) -> String {
        format!(
            "INSTALL\t= /usr/bin/env install\n\
                DEST\t=\n\
                PREFIX\t=\n\
                CC\t= {compiler}\n\
                BINDIR\t=\n\
                LIBDIR\t=\n\
                TARGET\t= {target}\n\
                LINK\t=\n\
                CFLAGS\t= {cflags}\n\
                LFLAGS\t=\n\
                LIBS\t= {libs}\n\
                INCDIR\t= {incdir}\n\
                LIBDIR\t=\n\
                CLEAN\t=\n\
                DEL\t= rm -f\n\
                {srcs}\n\n\
                first: all\n\n\
                .PHONY: doc clean\n\n\
                .SUFFIXES: .o .c .cpp .cc .cxx .C\n\n\
                .cpp.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\
                .cc.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                .cxx.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                .C.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                .c.o:\n\t$(CC) -c $(CFLAGS) $(INCDIR) -o \"$@\" \"$<\"\n\n\
                all: $(TARGET)\n\n\
                $(TARGET): $(OBJ)\n\
                \t$(CC) $(LFLAGS) -o $(TARGET) $(OBJ) $(LIBS)\n\n\
                {dep_list}\n\
                clean:\n\
                \t$(DEL) $(OBJ)\n\
                \t$(DEL) {target}\n\
                \t$(DEL) {clean_list}\n\
                ",
                compiler = &self.compiler.clone().unwrap_or("gcc".to_owned()),
                target = &self.target.clone().unwrap_or("a".to_owned()),
                cflags = &self.gen_make_cflags_list(),
                libs = &self.gen_make_lib_list(),
                incdir = &self.gen_make_inc_list(),
                srcs = &self.gen_make_src_list(),
                dep_list = &self.gen_make_src_deps(),
                clean_list = &self.concat_clean())
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
