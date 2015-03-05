// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;

use std::old_io::File;

#[derive(Default)]
pub struct Profile {
    pub os: String,
    pub lang: String,
    pub target: String,
    pub cc: String,
    pub cxx: String,
    pub dist: String,
    pub include: String,
    pub remote: String,
    pub arch: Vec<String>,
    pub cxxflags: Vec<String>,
    pub libs: Vec<String>,
    pub incdir: Vec<String>,
    pub clean: Vec<String>,
    pub lflags: Vec<String>,
    pub before: Vec<String>,
    pub after: Vec<String>,
    pub obj: Vec<String>,
}

impl Profile {
    pub fn from_file(self, file: Path) {
        let mut f = File::open(&file);
        let toml = f.read_to_string().unwrap();
        let val: toml::Value = toml.parse().unwrap();
        println!("{:?}", val);
    }
}
