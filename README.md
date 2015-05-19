# yabs
Yet another build system

[![Build Status](https://travis-ci.org/0X1A/yabs.svg)](https://travis-ci.org/0X1A/yabs)
[![Coverage Status](https://coveralls.io/repos/0X1A/yabs/badge.svg?branch=master)](https://coveralls.io/r/0X1A/yabs?branch=master)
[![Build Status on 0X1A](http://0x1a.us/img/yabs-build.png)](http://0x1a.us/ci/yabs)
[![Coverage Status on 0X1A](http://0x1a.us/img/yabs-coverage.png)](http://0x1a.us/ci/yabs)

A C and C++ build system for multiple architectures and operating systems.
Project files define Makefiles to be written that can be copied to a remote
system or be completely built on a remote system.

## Status
yabs can currently self host. Major things that are left to implement are:

- [ ] Remote jobs (ssh and or git)
- [ ] Automatic language detection
- [ ] Better distribution tarballs
- [ ] Pre/Post scripting
- [x] Direct building (omitting Make)

## Building

Use the `Makefile` provided, this compiles `yabs` with `g++` and gcov enabled.

## Using
```bash
~/Project $ cat hello-yabs.c
#include <stdio.h>

int main()
{
	printf("Hello yabs!\n");
	return 0;
}
~/Project $ yabs -n
New build file written as: Project.ybf
```
**Edit the build file to reflect the language used**
```bash
---
lang: c
...
```
**Build it**
```bash
~/Project $ yabs -b Project.ybf
g++ -c -I/usr/include -I/usr/local/include -o hello-yabs.o hello-yabs.c
g++ -o Project hello-yabs.o -L/usr/lib -L/usr/local/lib
~/Project $ ./Project
Hello yabs!

```

## Defining a Project

To learn how projects are defined please see the [projects.md](projects.md)

## Dependencies
- `libyaml`
- `libgit2`
- `libssh2`
- `libarchive`
- C++11 compliant compiler

## Contributing
Commits must be **signed**, gpg signing is optional. Patches mailed to the 
mailing list is preferable.

### Mailing list
`yabs@librelist.com`

## Releases
All releases are signed with keyid `639DEF0A`
