// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PROFILE_H
#define _PROFILE_H

#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include "gen.h"

#define MAX_OPT 25

typedef std::multimap<std::string, std::string> ProfileMultiMap;
typedef ProfileMultiMap::const_iterator ProfileConstIter;
typedef ProfileMultiMap::iterator ProfileIter;

class Profile : public Generate
{
private:
	FILE *inc_conf;
	FILE *Makefile;
	std::vector<std::string> obj;
	ProfileMultiMap ProfileMap;
	const std::string STDValues[MAX_OPT] = {
	    "os", "arch", "comp", "cflags", "lflags", "include", "lang", "dist",
	    "before-script", "after-script", "libs", "libdir", "incdir",
	    "target", "remote", "clean", "defines", "version", "ignore", "src",
	    "install", "uninstall", "doc", "doc-type",
	};
	std::string temp;

public:
	Profile();
	std::string ConvValue(unsigned char *conv_value);
	std::string PrependLink(std::string &to_pre, std::string pre);
	std::string GetOS() const;
	std::string MapValuesToString(std::string key);
	std::string VectToString(std::vector<std::string> &);
	int CompValid(unsigned char *comp_value);
	int WriteMake(const char *makefile);
	int Build();
	void OpenInclude(const std::string file);
	void PopValidValue(std::string &k_value, std::string v_value);
	void WriteVecValues(std::vector<std::string> &vect,
			    std::string out_name);
	void WriteMacroValues(const std::string &val, std::string out_name);
	void PrintProfile() const;
	void IgnorePath(std::vector<std::string> &vect);
	void SrcList();
	void CheckBlankValues();
	void CheckLang();
	void BuildObjList();
	void ExecScript(std::string);
	bool IsCmd(std::string &);
};

#endif
