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
use std::fmt::Display;
use std::process::exit;
use util::*;

enum ExitStatus {
    Success = 0,
    GeneralError,
    OptionError,
    BuildError,
}

fn check_error<T, E: Display>(result: Result<T, E>, status: ExitStatus) -> ExitStatus {
    if let Err(error) = result {
        error!("{}", error.to_string());
    }
    status
}

fn run() -> ExitStatus {
    let yaml = load_yaml!("cli.yaml");
    if let Err(error) = logger::Logger::init() {
        error!("{}", error.to_string());
        return ExitStatus::OptionError;
    };
    let matches = App::from_yaml(yaml).get_matches();
    if let Some(matches) = matches.subcommand_matches("new") {
        if matches.is_present("bin") {
            if let Some(value) = matches.value_of("bin") {
                if let Err(error) = new::new_project(&value.to_owned(), false) {
                    error!("{}", error.to_string());
                    return ExitStatus::OptionError;
                }
            }
        }
    } else if let Ok(ref mut cwd) = env::current_dir() {
        match build::find_build_file(cwd) {
            Ok(mut build_file) => {
                if let Some(matches) = matches.subcommand_matches("build") {
                    let mut jobs = num_cpus::get();
                    if let Some(jobs_given) = matches.value_of("jobs") {
                        match jobs_given.parse::<usize>() {
                            Ok(j) => jobs = j,
                            Err(error) => {
                                error!("{}", error.to_string());
                                return ExitStatus::OptionError;
                            },
                        }
                    }
                    if let Some(bin_name) = matches.value_of("bin") {
                        return check_error(build_file.build_binary_with_name(bin_name, jobs),
                                           ExitStatus::BuildError);
                    } else if let Some(lib_name) = matches.value_of("lib") {
                        return check_error(build_file.build_library_with_name(lib_name, jobs),
                                           ExitStatus::BuildError);
                    } else {
                        return check_error(build_file.build(jobs), ExitStatus::BuildError);
                    }
                } else if matches.subcommand_matches("clean").is_some() {
                    return check_error(build_file.clean(), ExitStatus::GeneralError);
                }
                if matches.is_present("sources") {
                    build_file.print_sources();
                }
            },
            Err(error) => {
                error!("{}", error.to_string());
                return ExitStatus::GeneralError;
            },
        }
    }
    ExitStatus::Success
}

fn main() {
    match run() {
        error => exit(error as i32),
    }
}
