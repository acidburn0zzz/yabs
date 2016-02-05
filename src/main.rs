// Copyright (c) 2015, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

static NAME: &'static str = "yabs";
static VERS: &'static str = "0.1.0";

extern crate pgetopts;
extern crate util;

use pgetopts::{Options};

use std::env;
use util::*;
use util::build::Desc;

fn print_usage(opts: Options) {
    println!("{}", opts.options());
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut opts = Options::new();
    let dsec = match build::ProjDesc::from_file("yabs.toml", "project") {
        Ok(_) => (),
        Err(e) => {
            for err in e {
                println!("error: {}", err.to_string());
            }
        },
    };

    opts.optflag("n", "new", "Create a new build profile");
    opts.optflag("h", "help", "Print help information");
    opts.optflag("", "version", "Print version information");

    let matches = match opts.parse(&args[1..]) {
        Ok(m) => { m },
        Err(e) => { panic!("{}", e.to_string()); }
    };

    if matches.opt_present("h") { print_usage(opts); }
    else if matches.opt_present("version") { println!("{} {}", NAME, VERS); }
}
