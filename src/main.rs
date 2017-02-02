// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate util;
#[macro_use]
extern crate clap;
#[macro_use]
extern crate log;

use clap::App;
use std::process::exit;
use util::*;
use util::error::YabsError;

fn print_error_vect(errors: Vec<YabsError>) {
    for error in errors {
        error!("{}", error.to_string());
    }
}

fn run() -> i32 {
    let yaml = load_yaml!("cli.yaml");
    if let Err(error) = logger::Logger::init() {
        error!("{}", error.to_string());
        return 1;
    };
    let matches = App::from_yaml(yaml).get_matches();
    if let Some(matches) = matches.subcommand_matches("new") {
        if matches.is_present("bin") {
            if let Some(value) = matches.value_of("bin") {
                if let Err(error) = new::new_project(&value.to_owned(), false) {
                    error!("{}", error.to_string());
                    return 1;
                }
            }
        }
    } else if let Some(mut assumed_file_name) = ext::get_assumed_filename() {
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
                    if let Err(error) = build_file.build(build.to_owned()) {
                        error!("{}", error.to_string());
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
