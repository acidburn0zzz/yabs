// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _DIST_H
#define _DIST_H

#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h>
#include <iostream>

extern int copy_data(struct archive *arch, struct archive *archive_write);
extern void extract(const char *proj_path);

#endif
