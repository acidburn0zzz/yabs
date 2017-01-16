// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;
extern crate walkdir;
extern crate ansi_term;
extern crate serde_json;

use ansi_term::Colour::Red;

use std::process::exit;
use std::io;
use std::error::*;
use std::fmt;
use std::string;

#[derive(Debug)]
pub enum YabsError {
    Io(io::Error),
    Toml(toml::Error),
    TomlParse(toml::ParserError),
    TomlDecode(toml::DecodeError),
    Utf8(string::FromUtf8Error),
    Json(serde_json::error::Error),
    WalkDir(walkdir::Error),
    Command(String, i32),
    NoLang(String),
    NoDesc(String),
    NoAssumedToml(String),
}

impl YabsError {
    pub fn exit_with_status(&self, status: i32) {
        println!("{} {}", Red.paint("error:"), &self);
        exit(status);
    }
}

impl fmt::Display for YabsError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            YabsError::Io(ref err) => write!(f, "I/O error, {}", err),
            YabsError::Toml(ref err) => write!(f, "toml error, {}", err),
            YabsError::TomlParse(ref err) => write!(f, "toml parsing error, {}", err),
            YabsError::TomlDecode(ref err) => write!(f, "toml decoding error, {}", err),
            YabsError::Utf8(ref err) => write!(f, "UTF-8 conversion error, {}", err),
            YabsError::Json(ref err) => write!(f, "json serialization error, {}", err),
            YabsError::WalkDir(ref err) => write!(f, "directory walking error, {}", err),
            YabsError::Command(ref cmd, ref exit_status) => write!(f, "command '{}' exited with status {}", cmd, exit_status),
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
            YabsError::Toml(ref err) => err.description(),
            YabsError::TomlParse(ref err) => err.description(),
            YabsError::TomlDecode(ref err) => err.description(),
            YabsError::Utf8(ref err) => err.description(),
            YabsError::Json(ref err) => err.description(),
            YabsError::WalkDir(ref err) => err.description(),
            YabsError::Command(..) => "command exited unsuccessfully",
            YabsError::NoLang(..) => "no language set in profile",
            YabsError::NoDesc(..) => "no desc",
            YabsError::NoAssumedToml(..) => "no assumed toml file",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            YabsError::Io(ref err) => Some(err),
            YabsError::Toml(ref err) => Some(err),
            YabsError::TomlParse(ref err) => Some(err),
            YabsError::TomlDecode(ref err) => Some(err),
            YabsError::Utf8(ref err) => Some(err),
            YabsError::Json(ref err) => Some(err),
            YabsError::WalkDir(ref err) => Some(err),
            YabsError::Command(..) => None,
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

impl From<toml::Error> for YabsError {
    fn from(err: toml::Error) -> YabsError {
        YabsError::Toml(err)
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

impl From<string::FromUtf8Error> for YabsError {
    fn from(err: string::FromUtf8Error) -> YabsError {
        YabsError::Utf8(err)
    }
}

impl From<serde_json::error::Error> for YabsError {
    fn from(err: serde_json::error::Error) -> YabsError {
        YabsError::Json(err)
    }
}

impl From<walkdir::Error> for YabsError {
    fn from(err: walkdir::Error) -> YabsError {
        YabsError::WalkDir(err)
    }
}

impl From<YabsError> for Vec<YabsError> {
    fn from(err: YabsError) -> Vec<YabsError> {
        vec![err]
    }
}
