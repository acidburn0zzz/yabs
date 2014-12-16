// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PROFILE_H
#define _PROFILE_H

#include <stdlib.h>
#include <string>
#include <vector>

#define MAX_OPT 15

class Profile
{
public:
	std::string os;
	std::string lang;
	std::string target;
	std::vector<std::string> arch;
	std::string cc;
	std::string cxx;
	std::string dist;
	std::string include;
	std::vector<std::string> libs;
	std::vector<std::string> incdir;
	std::vector<std::string> libdir;
	std::string remote;
	std::string defines;
	std::vector<std::string> before;
	std::vector<std::string> after;
	FILE *inc_conf;
	std::string plat;
	std::string p_arch;
	std::string STDValues[MAX_OPT] = {
	    "os",     "arch",   "cc",		 "cxx",		 "include",
	    "lang",   "dist",   "before-script", "after-script", "libs",
	    "incdir", "libdir", "target",	"remote",       "defines",
	};

public:
	Profile();
	int CompValid(unsigned char *comp_value);
	int PopLists(unsigned char *list_value);
	int RegValues(const char *reg_value);
	std::string ConvValue(unsigned char *conv_value);
	std::string PrependLink(std::string link, std::string pre);
	void OpenInclude(std::string file);
	void ParseKey(std::string key);
	void PopValidValue(std::string k_value, std::string v_value);
	void PrintList(std::vector<std::string> vect);
	void PrintProfile();
	void CheckBlankValues();
	void GetSysInfo();
};

#endif
