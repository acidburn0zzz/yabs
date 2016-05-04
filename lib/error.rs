// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;
extern crate walkdir;

use std::io;
use std::io::prelude::*;
use std::error::*;
use std::fmt;

#[derive(Debug)]
pub enum YabsError {
    Io(io::Error),
    TomlParse(toml::ParserError),
    TomlDecode(toml::DecodeError),
    WalkDir(walkdir::Error),
    NoLang(String),
    NoDesc(String),
    NoAssumedToml(String),
}

impl fmt::Display for YabsError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            YabsError::Io(ref err) => write!(f, "I/O error, {}", err),
            YabsError::TomlParse(ref err) => write!(f, "toml parsing error, {}", err),
            YabsError::TomlDecode(ref err) => write!(f, "toml decoding error, {}", err),
            YabsError::WalkDir(ref err) => write!(f, "directory walking error, {}", err),
            YabsError::NoLang(ref profile) => write!(f, "no language found in profile {}", profile),
            YabsError::NoDesc(ref name) => write!(f, "no '{}' section found in project file", name),
            YabsError::NoAssumedToml(ref name) => write!(f, "couldn't find file '{}'", name),
        }
    }
}

impl Error for YabsError {
    fn description(&self) -> &str {
        match *self {
            YabsError::Io(ref err) => err.description(),
            YabsError::TomlParse(ref err) => err.description(),
            YabsError::TomlDecode(ref err) => err.description(),
            YabsError::WalkDir(ref err) => err.description(),
            YabsError::NoLang(..) => "no language set in profile",
            YabsError::NoDesc(..) => "no desc",
            YabsError::NoAssumedToml(..) => "no assumed toml file",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            YabsError::Io(ref err) => Some(err),
            YabsError::TomlParse(ref err) => Some(err),
            YabsError::TomlDecode(ref err) => Some(err),
            YabsError::WalkDir(ref err) => Some(err),
            YabsError::NoLang(..) => None,
            YabsError::NoDesc(..) => None,
            YabsError::NoAssumedToml(..) => None,
        }
    }
}

impl From<io::Error> for YabsError {
    fn from(err: io::Error) -> YabsError {
        YabsError::Io(err)
    }
}

impl From<toml::ParserError> for YabsError {
    fn from(err: toml::ParserError) -> YabsError {
        YabsError::TomlParse(err)
    }
}

impl From<toml::DecodeError> for YabsError {
    fn from(err: toml::DecodeError) -> YabsError {
        YabsError::TomlDecode(err)
    }
}

impl From<walkdir::Error> for YabsError {
    fn from(err: walkdir::Error) -> YabsError {
        YabsError::WalkDir(err)
    }
}
