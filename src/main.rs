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

use pgetopts::Options;
use rpf::*;

use std::env;
use util::*;
use util::build::Desc;

fn print_usage(opts: Options) {
    print!("{0}: {1} {2} {3}\n   or: {1} {2}\nRun yabs with {2} on {3}",
           "Usage".bold(),
           YABS.name,
           "[OPTION]".underline(),
           "PROFILE".underline());
    print!("{}\n", opts.options());
    print!("{}\n\
           \tyabs -m linux\tGenerates a Makefile for the build profile 'linux'\n\
           \tyabs -p\t\tPrints all build profiles\n",
           "Examples:".bold());
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut opts = Options::new();
    opts.optflag("h", "help", "Print help information");
    opts.optopt("m", "make", "Generate Makefile", "PROFILE");
    opts.optflag("p", "print", "Print build file in JSON");
    opts.optopt("",
                "print-profile",
                "Print a particular profile from build file in JSON",
                "PROFILE");
    opts.optflag("", "profiles", "Print all available profiles in build file");
    opts.optflag("", "sources", "Print source files");
    opts.optflag("", "version", "Print version information");

    let matches = match opts.parse(&args[1..]) {
        Ok(m) => m,
        Err(e) => {
            YABS.error(e.to_string(), ExitStatus::OptError);
            panic!();
        }
    };

    if matches.opt_present("h") {
        print_usage(opts);
    } else if matches.opt_present("version") {
        YABS.copyright("Copyright (C) 2015-2016\n",
                       &["Alberto Corona"]);
    } else {
        if let Some(assumed_file_name) = ext::get_assumed_filename() {
            match build::BuildFile::from_file(&assumed_file_name) {
                Ok(build_file) => {
                    if matches.opt_present("p") {
                        &build_file.print_as_json();
                    } else if matches.opt_present("print-profile") {
                        if let Some(arg) = matches.opt_str("print-profile") {
                            &build_file.print_profile_as_json(arg);
                        }
                    } else if matches.opt_present("profiles") {
                        &build_file.print_available_profiles();
                    } else if matches.opt_present("sources") {
                        &build_file.clone().print_sources();
                    } else if matches.opt_present("make") {
                        if let Some(arg) = matches.opt_str("make") {
                            if let Err(e) = build_file.gen_make(arg) {
                                YABS.error(e.to_string(), ExitStatus::Error);
                            }
                        }
                    }
                }
                Err(e) => {
                    for err in e {
                        println!("{} {}", "yabs:".paint(Color::Red), err.to_string().paint(Color::Red));
                    }
                    YABS.exit(ExitStatus::Error);
                }
            };
        }
    }
}
