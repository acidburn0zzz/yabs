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
Nothing works the way it's mean to right now. c:

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
