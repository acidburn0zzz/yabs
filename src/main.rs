// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate util;
extern crate num_cpus;
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
    } else if let Ok(ref mut cwd) = env::current_dir() {
        match build::find_build_file(cwd) {
            Ok(mut build_file) => {
                if matches.is_present("build") {
                    let mut jobs = num_cpus::get();
                    if let Some(jobs_given) = matches.value_of("jobs") {
                        match jobs_given.parse::<usize>() {
                            Ok(j) => jobs = j,
                            Err(error) => {
                                error!("{}", error.to_string());
                                return 2;
                            },
                        }
                    }
                    if let Err(error) = build_file.build(jobs) {
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
