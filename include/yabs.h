// Copyright (c) 2013-2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _YABS_H
#define _YABS_H

#include "decoder.h"
#include "profile.h"

class Yabs : public Decoder, public Profile
{
public:
	Yabs();
	~Yabs();
};

Yabs::Yabs(){};
Yabs::~Yabs(){};

#endif
