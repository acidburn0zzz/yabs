# yabs
Yet another build system

[![Build Status](https://travis-ci.org/0X1A/yabs.svg)](https://travis-ci.org/0X1A/yabs) 
[![Coverage Status](https://img.shields.io/coveralls/0X1A/yabs.svg)](https://coveralls.io/r/0X1A/yabs?branch=master)
[![Build Status on 0X1A](http://0x1a.us/img/yabs-build.png)](http://0x1a.us/ci/yabs)
[![Coverage Status on 0X1A](http://0x1a.us/img/yabs-coverage.png)](http://0x1a.us/ci/yabs)

A build system for multiple architectures and operating systems. Project files 
define Makefiles to be written that can be copied to a remote system or be 
completely built on a remote system.

## Status
yabs can currently self host. Major things that are left to implement are:

- [ ] Remote jobs (ssh and or git)
- [ ] Better language detection (& add more languages)
- [ ] Better distribution tarballs
- [ ] Pre/Post scripting
- [x] Direct building (omitting Make)

Yabs currently only works well with C and C++ and only with certain file 
extensions

## Building

You may use the `Makefile` provided, a qmake profile is available if you wish to 
 compile with clang:
```
qmake "CONFIG+=clang"
```

## Defining a Project

To learn how projects are defined please see the [projects.md](projects.md)

## Dependencies
- `libyaml`
- `libgit2`
- `libssh2`
- C++11 compliant compiler

## Contributing
Commits must be **signed**, gpg signing is optional. Patches mailed to the 
mailing list is preferable.

### Mailing list
`yabs@librelist.com`

## Releases
All releases are signed with keyid `639DEF0A`
