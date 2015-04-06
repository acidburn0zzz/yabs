// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#![feature(std_misc,collections, path_ext)]
#![allow(unused_imports)]

mod gen;
mod profile;
use std::env;
use std::default::Default;
use std::path::PathBuf;
use gen::Gen;
use profile::Profile;

fn main() {
    let mut gen: Gen = Default::default();
    let mut prof: Profile = Default::default();
    let test = PathBuf::from("yabs.toml");
    gen.is_dot(&test);
    gen.has_ext(&test, &String::from_str("toml"));
    gen.walk_dir(env::current_dir().unwrap(), &String::from_str("rs"));
    gen.print_filelist();
}
