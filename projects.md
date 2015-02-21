## Defining a Project
Project files for yabs are written in yaml. Currently, anchors, aliases and tags 
are not properly
interpreted.

A valid project file would look something like:
```YAML
---
os: linux
target: yabs
include: yabs-include.yml
libs: yaml archive gcov
remote: git@linux.fqdn.com:yabs-test-linux
lang: c++
arch:
 - x86_64
 - i686
cc: gcc
cxx: g++
dist: yabs.tar.gz
before-script:
 - sudo pacman -S libyaml libssh2 libgit2
after-script:
 - make -f Make.linux-x86_64
 - ./yabs --help
...
```
Where the key `include` would include another file with more values 
for the project, here `yabs-include.yml` is:
```YAML
libs: yaml ssh2 git2 archive
inc: /usr/include /usr/local/include
libdir: /usr/lib /usr/local/lib
```
Libraries used in the project do not have to be prepended with `-l` but they do 
have to be valid library files. This also extends to include and library 
directories where these would normally be prepended with `-L` and `-I` in a 
`Makefile`.

## Keys and Values

Valid options for a project file are as follows:
```
- os: Operating system the project is being built for
- remote: Remote that this project could either be built on or pushed to (git repos)
- lang: The language the project is written in. Currently only C and C++ are supported
- arch: The architecture the project is being built for
- incdir: Directories to include in Makefile
- libs: Libraries to link. These should not be prepended with '-l'
- libdir: Library directory
- cc: The C compiler to be used
- cxx: The C++ compiler to be used
- dist: The distribution tarball
- {before, after}-script: Commads to be run either before the project's Makefile is written or after
- clean: Items to removed on `make clean`
- defines: Defines to include in Makefile
```

## Multiple Documents
Multiple documents in a yaml file can define multiple Makefiles, and inherently,
multiple operating systems, for example:
```YAML
---
os: linux
target: yabs
include: yabs-include.yml
libs: yaml archive gcov
remote: git@linux.fqdn.com:yabs-test-linux
lang: c++
arch:
 - x86_64
 - i686
cc: gcc
cxx: g++
dist: yabs.tar.gz
before-script:
 - sudo pacman -S libyaml libssh2 libgit2
after-script:
 - make -f Make.linux-x86_64
 - ./yabs --help
...
---
os: freebsd
target: yabs-bsd
include: yabs-include.yml
libs: yaml archive gcov
remote: git@bsd.fqdn.com:yabs-test-bsd
lang: c++
arch:
cc: clang
cxx: clang++
dist: yabs-bsd.tar.gz
before-script:
 - sudo pkg in libyaml libssh2 libgit2
after-script:
 - make -f Makefile.bsd-x86_64
...
```
Note that the Makefiles produced are apended with the name of the operating 
system and if multiple architectures for that operting system are defined 
the architecture is also apended. As such the Makefiles produced by the project
file about would be `Makefile.linux-x86_64`,`Makefile.linux-i686`, 
and `Makefile.bsd-x86_64`. If no architecure is defined then the architecture 
of the host is apended.
