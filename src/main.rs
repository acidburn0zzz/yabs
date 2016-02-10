// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

pub static YABS: Prog = Prog {
    name: "yabs",
    vers: "0.1.0",
    yr: "2016",
};

extern crate pgetopts;
extern crate util;
extern crate rpf;

use pgetopts::{Options};
use rpf::*;

use std::env;
use util::*;
use util::build::Desc;

fn print_usage(opts: Options) {
    println!("{}", opts.options());
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut opts = Options::new();
    opts.optflag("n", "new", "Create a new build profile");
    opts.optflag("h", "help", "Print help information");
    opts.optflag("", "version", "Print version information");
    opts.optflag("p", "print", "Print build file in JSON");
    opts.optopt("","print-profile", "Print a particular profile from build file in JSON", "PROFILE \
                where profile is the name of the profile to be printed");

    let matches = match opts.parse(&args[1..]) {
        Ok(m) => { m },
        Err(e) => {
            YABS.error(e.to_string(), ExitStatus::OptError);
            panic!();
        }
    };

    if matches.opt_present("h") { print_usage(opts); }
    else if matches.opt_present("version") { println!("{} {}", YABS.name, YABS.vers); }
    else {
        if let Some(assumed_file_name) = ext::get_assumed_filename() {
            if matches.opt_present("p") {
                match build::BuildFile::from_file(&assumed_file_name) {
                    Ok(k) => {
                        &k.print_as_json();
                    },
                    Err(e) => {
                        for err in e {
                            println!("error: {}", err.to_string());
                        }
                        YABS.exit(ExitStatus::Error);
                    }
                };
            } else if matches.opt_present("print-profile") {
                match build::BuildFile::from_file(&assumed_file_name) {
                    Ok(file) => {
                        if let Some(arg) = matches.opt_str("print-profile") {
                            &file.print_profile_as_json(arg);
                        }
                    } Err(e) => {
                        for err in e {
                            println!("error: {}", err.to_string());
                        }
                        YABS.exit(ExitStatus::Error);
                    }
                }
            }
        }
    }
}
