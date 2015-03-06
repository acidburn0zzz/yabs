// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#![feature(path,collections,os,fs)]

mod gen;
mod profile;
use std::env;
use std::default::Default;
use std::path::PathBuf;
use gen::Gen;
use profile::Profile;

fn main() {
    let current = &env::current_dir().unwrap();
    let mut generator: Gen = Default::default();
    generator.recur_walk(current, current, &String::from_str("rs"));
    generator.print_file_list();
    let profile: Profile = Default::default();
    profile.from_file(PathBuf::new("yabs.toml"));
}
