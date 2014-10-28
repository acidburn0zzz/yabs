// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <string.h>
#include <yaml.h>
#include <iostream>
#include <string>
#include <vector>
#include "profile.h"

Profile::Profile()
{
	DocNum = 0;
}

void Profile::IncDocNum()
{
	DocNum++;
}

int Profile::AssertConfig(unsigned char *value)
{
	if (CompValid(value) != 1) {
		printf("Error: invalid configuration option: %s\n", value);
		return -1;
	}
	return 0;
}

const char *Profile::ConvValue(unsigned char *conv_value)
{
	std::string temp_value;
	temp_value.append(reinterpret_cast<const char *>(conv_value));
	return temp_value.c_str();
}

int Profile::CompValid(unsigned char *comp_value)
{
	for (int i = 0; i <= MAX_OPT - 1; i++) {
		if (strcasecmp(STDValues[i], ConvValue(comp_value)) == 0) {
			return 1;
		}
	}
	return 0;
}

const char *Profile::PrependLink(const char *string, const char *pre)
{
	std::string temp_string = string;
	std::string temp_pre = pre;
	std::string n_pre = " ";
	size_t start_pos = 0;
	while ((start_pos = temp_string.find(n_pre, start_pos)) != std::string::npos) {
		temp_string.replace(start_pos, n_pre.length(), temp_pre);
		start_pos += temp_pre.length();
	}
	return temp_string.c_str();
}

void Profile::OpenInclude(const char *file)
{
	inc_conf = fopen(file, "r");
	if (inc_conf == NULL) {
		printf("Error: Couldn't open included file: %s\n", file);
	}
}

void Profile::PopValidValue(std::string &k_value, const char *v_value)
{
	if (strcasecmp("os", k_value.c_str()) == 0) {
		OSList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("arch", k_value.c_str()) == 0) {
		ArchList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("cc", k_value.c_str()) == 0) {
		CCList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("cxx", k_value.c_str()) == 0) {
		CXXList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("include", k_value.c_str()) == 0) {
		ConfIncList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("lang", k_value.c_str()) == 0) {
		LangList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("dist", k_value.c_str()) == 0) {
		DistList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("before-script", k_value.c_str()) == 0) {
		BeforeScriptList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("after-script", k_value.c_str()) == 0) {
		AfterScriptList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("libs", k_value.c_str()) == 0) {
		LibsList.push_back(PrependLink(v_value, " -l"));
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("incdir", k_value.c_str()) == 0) {
		IncDirList.push_back(PrependLink(v_value, " -I"));
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("libdir", k_value.c_str()) == 0) {
		LibDirList.push_back(PrependLink(v_value, " -L"));
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("target", k_value.c_str()) == 0) {
		TargetList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("remote", k_value.c_str()) == 0) {
		RemoteList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
	if (strcasecmp("defines", k_value.c_str()) == 0) {
		DefinesList.push_back(v_value);
		printf("Adding value: %s to key: %s\n", v_value, k_value.c_str());
	}
}
