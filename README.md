# yabs
Yet another build system

A C and C++ build system for multiple architectures and operating systems.
Project files define Makefiles to be written that can be copied to a remote
system or be completely built on a remote system.


# Deprecated
This version of yabs will no longer be developed. All current work is being
done on porting yabs to Rust. There are issues with yabs that do not allow it
to be easily extensible. All development will now take place in the branch
`rust-port`. Once the port is completed, `rust-port` will be marged with the
master branch and this version will no longer be maintained.

---

## Status
yabs can currently self host. Major things that are left to implement are:

- [ ] Automatic language detection
- [x] Pre/Post scripting
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
