// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PLATDEF
#define _PLATDEF

#ifdef __unix__
int unix_def = 1;
int win_def = 0;
#endif

#ifdef __WIN32__
int unix_def = 0;
int win_def = 1;
#endif

#endif
