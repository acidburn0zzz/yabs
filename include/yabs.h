// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _YABS_H
#define _YABS_H

#include "gen.h"
#include "parser.h"

class Yabs : public Generate, public Parser
{
public:
	Yabs();
	~Yabs();
};

Yabs::Yabs(){};
Yabs::~Yabs(){};

#endif
