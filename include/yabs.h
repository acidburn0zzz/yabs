// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _YABS_H
#define _YABS_H

#include "parser.h"
#include "profile.h"

class Yabs : public Parser, public Profile
{
public:
	Yabs();
	~Yabs();
};

Yabs::Yabs(){};
Yabs::~Yabs(){};

#endif
