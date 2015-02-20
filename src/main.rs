// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#![feature(env,io,core,path)]

mod gen;
mod profile;
use std::env;
use std::default::Default;
use gen::Gen;
use profile::Profile;

fn main() {
    let mut generator: Gen = Default::default();
    generator.recur_walk(&env::current_dir().unwrap());
    //generator.print_file_list();
    let mut profile: Profile = Default::default();
    profile.from_file(Path::new("yabs.toml"));
}
