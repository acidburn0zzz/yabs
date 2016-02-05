extern crate toml;

use std::io;
use std::io::prelude::*;
use std::error::*;
use std::fmt;
use std::fmt::Display;

#[derive(Debug)]
pub enum YabsError {
    Io(io::Error),
    TomlParse(toml::ParserError),
    TomlDecode(toml::DecodeError),
    NoDesc(String),
}

impl fmt::Display for YabsError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            YabsError::Io(ref err) => write!(f, "I/O error, {}", err),
            YabsError::TomlParse(ref err) => write!(f, "toml parsing error, {}", err),
            YabsError::TomlDecode(ref err) => write!(f, "toml decoding error, {}", err),
            YabsError::NoDesc(ref name) => write!(f, "no '{}' section found in project file", name),
        }
    }
}

impl Error for YabsError {
    fn description(&self) -> &str {
        match *self {
            YabsError::Io(ref err) => err.description(),
            YabsError::TomlParse(ref err) => err.description(),
            YabsError::TomlDecode(ref err) => err.description(),
            YabsError::NoDesc(..) => "no desc",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            YabsError::Io(ref err) => Some(err),
            YabsError::TomlParse(ref err) => Some(err),
            YabsError::TomlDecode(ref err) => Some(err),
            YabsError::NoDesc(..) => None
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
