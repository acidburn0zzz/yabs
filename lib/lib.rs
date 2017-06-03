// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#![recursion_limit = "1024"]

#[macro_use]
extern crate serde_derive;
extern crate toml;
extern crate walkdir;
extern crate ansi_term;
extern crate serde;
#[macro_use]
extern crate error_chain;
extern crate serde_json;
#[macro_use]
extern crate log;

pub mod build;
pub mod error;
pub mod ext;
pub mod new;
pub mod logger;
mod desc;
