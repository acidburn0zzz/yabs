// Copyright (c) 2015 - 2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

extern crate error_chain;
extern crate toml;
extern crate walkdir;
extern crate ansi_term;
extern crate serde_json;
extern crate log;

error_chain! {
    types {
        YabsError, YabsErrorKind, Result;
    }

    foreign_links {
        Io(::std::io::Error);
        TomlDe(::toml::de::Error);
        WalkDir(::walkdir::Error);
        SetLog(::log::SetLoggerError);
        UTF8(::std::string::FromUtf8Error);
    }

    errors {
        NoAssumedToml(path: String) {
            description("no assumed toml file found")
                display("could not find assumed toml file: '{}'", path)
        }
        Command(cmd: String, status: i32) {
            description("command exited unsuccessfully")
                display("command '{}' exited with status '{}'", cmd, status)
        }
        DirExists(path: ::std::path::PathBuf) {
            description("directory already exists")
                display("directory '{}' already exists", path.display())
        }
    }
}
