// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate toml;
extern crate walkdir;
extern crate ansi_term;
extern crate serde_json;
extern crate log;

use std::error::*;
use std::fmt;
use std::io;
use std::path::PathBuf;
use std::string;

#[derive(Debug)]
pub enum YabsError {
    Io(io::Error),
    TomlSer(toml::ser::Error),
    TomlDe(toml::de::Error),
    Utf8(string::FromUtf8Error),
    Json(serde_json::error::Error),
    Log(log::SetLoggerError),
    WalkDir(walkdir::Error),
    Command(String, i32),
    NoLang(String),
    NoDesc(String),
    NoAssumedToml(String),
    DirExists(PathBuf),
}

impl fmt::Display for YabsError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            YabsError::Io(ref err) => write!(f, "I/O error, {}", err),
            YabsError::TomlSer(ref err) => write!(f, "toml serialization error, {}", err),
            YabsError::TomlDe(ref err) => write!(f, "toml deserialization error, {}", err),
            YabsError::Utf8(ref err) => write!(f, "UTF-8 conversion error, {}", err),
            YabsError::Json(ref err) => write!(f, "json serialization error, {}", err),
            YabsError::Log(ref err) => write!(f, "log set error, {}", err),
            YabsError::WalkDir(ref err) => write!(f, "directory walking error, {}", err),
            YabsError::Command(ref cmd, ref exit_status) => {
                write!(f, "command '{}' exited with status {}", cmd, exit_status)
            }
            YabsError::NoLang(ref profile) => write!(f, "no language found in profile {}", profile),
            YabsError::NoDesc(ref name) => write!(f, "no '{}' section found in project file", name),
            YabsError::NoAssumedToml(ref name) => write!(f, "couldn't find file '{}'", name),
            YabsError::DirExists(ref dir) => {
                write!(f, "directory '{}' already exists", dir.display())
            }
        }
    }
}

impl Error for YabsError {
    fn description(&self) -> &str {
        match *self {
            YabsError::Io(ref err) => err.description(),
            YabsError::TomlSer(ref err) => err.description(),
            YabsError::TomlDe(ref err) => err.description(),
            YabsError::Utf8(ref err) => err.description(),
            YabsError::Json(ref err) => err.description(),
            YabsError::Log(ref err) => err.description(),
            YabsError::WalkDir(ref err) => err.description(),
            YabsError::Command(..) => "command exited unsuccessfully",
            YabsError::NoLang(..) => "no language set in profile",
            YabsError::NoDesc(..) => "no desc",
            YabsError::NoAssumedToml(..) => "no assumed toml file",
            YabsError::DirExists(..) => "directory already exists",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            YabsError::Io(ref err) => Some(err),
            YabsError::TomlSer(ref err) => Some(err),
            YabsError::TomlDe(ref err) => Some(err),
            YabsError::Utf8(ref err) => Some(err),
            YabsError::Json(ref err) => Some(err),
            YabsError::Log(ref err) => Some(err),
            YabsError::WalkDir(ref err) => Some(err),
            YabsError::Command(..) => None,
            YabsError::NoLang(..) => None,
            YabsError::NoDesc(..) => None,
            YabsError::NoAssumedToml(..) => None,
            YabsError::DirExists(..) => None,
        }
    }
}

impl From<io::Error> for YabsError {
    fn from(err: io::Error) -> YabsError {
        YabsError::Io(err)
    }
}

impl From<toml::ser::Error> for YabsError {
    fn from(err: toml::ser::Error) -> YabsError {
        YabsError::TomlSer(err)
    }
}

impl From<toml::de::Error> for YabsError {
    fn from(err: toml::de::Error) -> YabsError {
        YabsError::TomlDe(err)
    }
}

impl From<log::SetLoggerError> for YabsError {
    fn from(err: log::SetLoggerError) -> YabsError {
        YabsError::Log(err)
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
