// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PROFILE_H
#define _PROFILE_H

#include <stdlib.h>
#include <string>
#include <vector>
#include "gen.h"

#define MAX_OPT 25

class Profile : public Generate
{
private:
	FILE *inc_conf;
	FILE *Makefile;
	std::string os;
	std::string lang;
	std::string target;
	std::string comp;
	std::string dist;
	std::string include;
	std::string remote;
	std::string defines;
	std::string version;
	std::vector<std::string> arch;
	std::vector<std::string> ignore;
	std::vector<std::string> cflags;
	std::vector<std::string> src;
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
	    "os",	    "arch",	 "comp",           "cflags",
	    "lflags",       "include", "lang",    "dist",
	    "before-script", "after-script", "libs",    "libdir",  "incdir",
	    "target",	"remote",       "clean",   "defines", "version",
	    "ignore",	"src", "install", "uninstall", "doc",
	};
	std::string temp;

public:
	Profile();
	std::string ConvValue(unsigned char *conv_value);
	std::string PrependLink(std::string &to_pre, std::string pre);
	std::string GetOS() const;
	std::string VectToString(std::vector<std::string> &to_swap);
	int CompValid(unsigned char *comp_value);
	int WriteMake(const char *makefile);
	int Build();
	void OpenInclude(const std::string file);
	void PopValidValue(std::string &k_value, std::string v_value);
	void PrintList(const std::vector<std::string> vect) const;
	void WriteListToMake(std::vector<std::string> &vect,
			     std::string out_name);
	void WriteSwapValues(const std::string &val, std::string out_name);
	void PrintProfile() const;
	void CleanList(std::vector<std::string> &vect);
	void SrcList();
	void CheckBlankValues();
	void GetSysInfo();
	void CheckLang();
	void BuildObjList();
	void ExecScript(std::vector<std::string> &) const;
};

#endif
