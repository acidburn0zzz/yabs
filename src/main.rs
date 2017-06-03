// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate util;
#[macro_use]
extern crate clap;
#[macro_use]
extern crate log;

use clap::App;
use std::env;
use std::process::exit;
use util::*;

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
    }
    if let Ok(ref mut cwd) = env::current_dir() {
        match build::find_build_file(cwd) {
            Ok(mut build_file) => {
                if matches.is_present("build") {
                    if let Err(error) = build_file.build() {
                        error!("{}", error.to_string());
                        return 2;
                    }
                }
                if matches.is_present("sources") {
                    &build_file.print_sources();
                }
                if matches.is_present("clean") {
                    if let Err(error) = build_file.clean() {
                        error!("{}", error.to_string());
                        return 2;
                    }
                }
            },
            Err(error) => {
                error!("{}", error.to_string());
                return 2;
            },
        }
    }
    0
}

fn main() {
    match run() {
        error @ 1...10 => exit(error),
        _ => (),
    }
}
