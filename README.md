# yabs
Yet another build system. A C and C++ build system, with projects described
using TOML.

[![](https://gitlab.com/0X1A/yabs/badges/master/build.svg)](https://gitlab.com/0X1A/yabs/builds)

## Building
To build `yabs` you will need `rustc` and `cargo`. Simply run `cargo build
--release`, this will build a binary called `yabs` in `target/release`.

# Using
Output of `yabs -h`
```
Usage: yabs [OPTION] PROFILE
   or: yabs [OPTION]
Run yabs with [OPTION] on PROFILE

Options:
    -f, --file FILE     Use a specified TOML file
    -h, --help          Print help information
    -m, --make PROFILE  Generate Makefile
    -p, --print         Print build file in JSON
    -b, --build         Build profile
        --print-profile PROFILE
                        Print a particular profile from build file in JSON
        --profiles      Print all available profiles in build file
        --sources       Print source files
        --version       Print version information

Examples:
	yabs -m linux	Generates a Makefile for the build profile 'linux'
	yabs -p		Prints all build profiles

```

## Defining a Project
`yabs` uses TOML to define projects. For example, a project in C++ using libraries such as SDL2, SDL2-image, SDL2-ttf, and Lua 5.3 would look similar to the following:

```toml
[linux.project]
name = "kuma"
target = "kuma"
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
static_lib = false

[static_lib.project]
name = "kuma"
static_lib = true
arflags = "rcs"
target = "libkuma.a"
lang = "cpp"
comp = "g++"
cflags = ["std=c++11"]
inc = ["src", "/usr/include/SDL2",]
libs = [
	"SDL2",
	"SDL2_image",
	"SDL2_ttf",
	"lua",
]
ignore = [
	"src/main.cpp",
	"tests/",
]
```

Here `[linux.project]` defines a toml table, which then defines the project corresponding to the key 'linux'.

### Building a Project
`yabs` can build a project directly though this does not support multiple jobs. In the prior example two projects were defined: `linux` and `static_lib`. To build `linux` one would simply run `yabs -b linux`.

### Generating a Makefile
`yabs` can also generate Makefiles for projects. This can be done with `-m`, using our previous example: `yabs -m linux`. This would create a Makefile for our project `linux`.

# Keys and Values
The following tables describes what keys are available to yabs project files.

| Key    | Value                           | Type |
| ---    | -----                           | ---- |
| `name`   | Name for project                | String |
| `target` | Name for target binary          | String |
| `lang`   | Extension used for source files | String |
| `os` | Operating system | String |
| `version` | Version number | String |
| `compiler` | Compiler to use | String |
| `src` | Source files | Array |
| `libs` | Libraries to link | Array |
| `lib_dir` | Library directories to use | Array |
| `inc` | Include directories | Array |
| `cflags` | Compiler flags | Array |
| `explicit_cflags` | Flags to be written without being altered | Array |
| `lflags` | Linker flags | Array |
| `ignore` | Directories or files to ignore | Array |
| `before_script` | Scripts to run before a build | Array |
| `after_script` |  Scripts to run after a build | Array |
| `static_lib` | Whether the project is a static library | Boolean |
| `ar` | Archiving tool to use | String |
| `arflags` | Flags for archiving tool | Array |
| `clean` | Extra items to clean, these are removed using `rm -r` | Array |
