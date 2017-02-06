# yabs
Yet another build system. A C and C++ build system, with projects described
using TOML.

[![Build Status](https://travis-ci.org/0X1A/yabs.svg?branch=master)](https://travis-ci.org/0X1A/yabs)

## Get Yabs
To install yabs simply run `cargo install yabs`. If you are installing from 
crates.io please see the version of documentation available on docs.rs that corresponds to
your output of `yabs --version`.

## Building
To build `yabs` you will need `rustc` and `cargo`. Simply run `cargo build
--release`, this will build a binary called `yabs` in `target/release`.

# Using
Output of `yabs -h`

```
yabs 0.1.2
Yet another build system

USAGE:
    yabs [FLAGS] [OPTIONS] [SUBCOMMAND]

FLAGS:
    -h, --help        Prints help information
        --profiles    Print available profiles
        --sources     Print source files for all profiles
    -V, --version     Prints version information

OPTIONS:
    -b, --build <PROFILE>    Build a profile
    -f, --file <FILE>        Use a specified TOML file
    -m, --make <PROFILE>     Generate Makefile for a profile
    -p, --print <PROFILE>    Print a build profile

```

## Defining a Project
`yabs` uses TOML to define projects. For example, a project in C++ using libraries such as SDL2, SDL2-image, SDL2-ttf, and Lua 5.3 would look similar to the following:

```toml
[linux.project]
name = "kuma"
target = ["kuma"]
lang = "cpp"
compiler = "g++"
cflags = ["std=c++11"]
inc = [
    "src",
    "`pkg-config --cflags lua5.3 SDL2_image SDL2_ttf`",
]
libs = [
        "`pkg-config --libs lua5.3 SDL2_image SDL2_ttf`",
]
ignore = [
	"tests/",
]
static-lib = false

[static_lib.project]
name = "kuma"
static-lib = true
arflags = "rcs"
target = ["libkuma.a"]
lang = "cpp"
comp = "g++"
cflags = ["std=c++11"]
inc = ["src", "/usr/include/SDL2",]
libs = [
	"SDL2",
	"SDL2-image",
	"SDL2-ttf",
	"lua",
]
ignore = [
	"src/main.cpp",
	"tests/",
]
```

Here `[linux.project]` defines a toml table, which then defines the project corresponding to the key 'linux'.

### Building a Project
`yabs` can build a project directly though this does not support multiple jobs. In the prior example two projects were defined: `linux` and `static-lib`. To build `linux` one would simply run `yabs -b linux`.

### Generating a Makefile
`yabs` can also generate Makefiles for projects. This can be done with `-m`, using our previous example: `yabs -m linux`. This would create a Makefile for our project `linux`.

# Keys and Values
The following tables describes what keys are available to yabs project files.

| Key    | Value                           | Type |
| ---    | -----                           | ---- |
| `name`   | Name for project                | String |
| `target` | Name for target binary          | Array |
| `lang`   | Extension used for source files | String |
| `os` | Operating system | String |
| `version` | Version number | String |
| `compiler` | Compiler to use | String |
| `src` | Source files | Array |
| `libs` | Libraries to link | Array |
| `lib-dir` | Library directories to use | Array |
| `inc` | Include directories | Array |
| `cflags` | Compiler flags | Array |
| `lflags` | Linker flags | Array |
| `ignore` | Directories or files to ignore | Array |
| `before-script` | Scripts to run before a build | Array |
| `after-script` |  Scripts to run after a build | Array |
| `static-lib` | Whether the project is a static library | Boolean |
| `ar` | Archiving tool to use | String |
| `arflags` | Flags for archiving tool | Array |
| `clean` | Extra items to clean, these are removed using `rm -r` | Array |
