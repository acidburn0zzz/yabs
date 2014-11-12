// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PROFILE_H
#define _PROFILE_H

#include <stdlib.h>
#include <string>
#include <vector>

#define MAX_OPT 15
#ifdef __unix__
#endif

class Profile
{
private:
	std::vector<std::string> OSList;
	std::vector<std::string> ArchList;
	std::vector<std::string> CCList;
	std::vector<std::string> CXXList;
	std::vector<std::string> ConfIncList;
	std::vector<std::string> LangList;
	std::vector<std::string> DistList;
	std::vector<std::string> BeforeScriptList;
	std::vector<std::string> AfterScriptList;
	std::vector<std::string> LibsList;
	std::vector<std::string> IncDirList;
	std::vector<std::string> LibDirList;
	std::vector<std::string> TargetList;
	std::vector<std::string> RemoteList;
	std::vector<std::string> DefinesList;
	int DocNum;
	FILE *inc_conf;
	std::string STDValues[MAX_OPT] = {
		"os", "arch", "cc", "cxx",
		"include", "lang", "dist", "before-script",
		"after-script", "libs", "incdir", "libdir",
		"target", "remote", "defines",
	};
	std::string temp_value;

public:
	Profile();
	int AssertConfig(unsigned char *value);
	int CompValid(unsigned char *comp_value);
	int PopLists(unsigned char *list_value);
	int RegValues(const char *reg_value);
	std::string ConvValue(unsigned char *conv_value);
	std::string PrependLink(std::string link, std::string pre);
	std::string PrintProfile();
	void OpenInclude(std::string file);
	void ParseKey(std::string key);
	void IncDocNum();
	void PopValidValue(std::string &k_value, std::string v_value);
	void PrintList(std::vector<std::string> vect);
};

#endif
