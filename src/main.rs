// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate util;
#[macro_use]
extern crate clap;
extern crate ansi_term;

use clap::App;
use std::process::exit;
use ansi_term::Colour::{Red, White};
use util::*;
use util::error::YabsError;

fn print_error(error: YabsError) {
    println!("{} {}",
             Red.bold().paint("error:"),
             White.bold().paint(error.to_string()));
}

fn print_error_vect(errors: Vec<YabsError>) {
    for error in errors {
        println!("{} {}",
                 Red.bold().paint("error:"),
                 White.bold().paint(error.to_string()));
    }
}

fn run() -> i32 {
    let yaml = load_yaml!("cli.yaml");
    let matches = App::from_yaml(yaml).get_matches();
    if let Some(mut assumed_file_name) = ext::get_assumed_filename() {
        if let Some(file) = matches.value_of("file") {
            assumed_file_name = file.to_owned();
        }
        match build::BuildFile::from_file(&assumed_file_name) {
            Ok(mut build_file) => {
                if let Some(print) = matches.value_of("print") {
                    &build_file.print_profile_as_json(print.to_owned());
                }
                if let Some(makefile) = matches.value_of("make") {
                    &build_file.gen_make(makefile.to_owned());
                }
                if let Some(build) = matches.value_of("build") {
                    if let Err(err) = build_file.build(build.to_owned()) {
                        print_error(err);
                        return 2;
                    }
                }
                if matches.is_present("sources") {
                    &build_file.print_sources();
                }
                if matches.is_present("profiles") {
                    &build_file.print_available_profiles();
                }
            },
            Err(err) => {
                print_error_vect(err);
                return 2;
            },
        };
    }
    0
}

fn main() {
    match run() {
        error @ 1...10 => exit(error),
        _ => (),
    }
}
