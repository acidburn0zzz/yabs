// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PROFILE_H
#define _PROFILE_H

#include <stdlib.h>
#include <string>
#include <vector>
#include "gen.h"

#define MAX_OPT 20

class Profile : public Generate
{
private:
	FILE *inc_conf;
	FILE *Makefile;
	std::string os;
	std::string lang;
	std::string target;
	std::string cc;
	std::string cxx;
	std::string dist;
	std::string include;
	std::string remote;
	std::string defines;
	std::vector<std::string> arch;
	std::vector<std::string> cxxflags;
	std::vector<std::string> libs;
	std::vector<std::string> incdir;
	std::vector<std::string> libdir;
	std::vector<std::string> clean;
	std::vector<std::string> lflags;
	std::vector<std::string> before;
	std::vector<std::string> after;
	std::vector<std::string> obj;
	std::string plat;
	std::string p_arch;
	std::string STDValues[MAX_OPT] = {
	    "os",	    "arch",	 "cc",      "cxx",     "cflags",
	    "cxxflags",      "lflags",       "include", "lang",    "dist",
	    "before-script", "after-script", "libs",    "libdir",  "incdir",
	    "target",	"remote",       "clean",   "defines",
	};
	std::string temp;

public:
	Profile();
	int CompValid(unsigned char *comp_value);
	int PopLists(unsigned char *list_value);
	int RegValues(const char *reg_value);
	int WriteMake(const char *makefile);
	std::string ConvValue(unsigned char *conv_value);
	std::string PrependLink(std::string link, std::string pre);
	std::string GetOS() const;
	void OpenInclude(std::string file);
	void ParseKey(std::string key);
	void PopValidValue(std::string k_value, std::string v_value);
	void PrintList(std::vector<std::string> vect) const;
	void PrintProfile() const;
	void CheckBlankValues();
	void GetSysInfo();
	void CheckLang();
	void BuildObjList();
};

#endif
