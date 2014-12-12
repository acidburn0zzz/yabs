// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <string.h>
#include <yaml.h>
#include <iostream>
#include <string>
#include <vector>
#include "profile.h"

using std::string;

Profile::Profile() {}

string Profile::ConvValue(unsigned char *conv_value)
{
	string temp_value;
	temp_value.append(reinterpret_cast<const char *>(conv_value));
	return temp_value;
}

int Profile::CompValid(unsigned char *comp_value)
{
	for (int i = 0; i <= MAX_OPT - 1; i++) {
		if (strcasecmp(STDValues[i].c_str(),
			       ConvValue(comp_value).c_str()) == 0) {
			return 1;
		}
	}
	return 0;
}

string Profile::PrependLink(string to_pre, string pre)
{
	string n_pre = " ";
	to_pre = pre + to_pre;
	pre = n_pre + pre;
	size_t start_pos = 0;
	while ((start_pos = to_pre.find(n_pre, start_pos)) != string::npos) {
		to_pre.replace(start_pos, n_pre.length(), pre);
		start_pos += pre.length();
	}
	return to_pre;
}

void Profile::OpenInclude(string file)
{
	inc_conf = fopen(file.c_str(), "r");
	if (inc_conf == NULL) {
		printf("Error: Couldn't open included file: %s\n",
		       file.c_str());
	}
}

void Profile::PrintList(std::vector<string> vect)
{
	for (unsigned int i = 0; i < vect.size(); i++) {
		std::cout << vect[i] << std::endl;
	}
}

void Profile::PrintProfile()
{
	printf("os: %s\n", os.c_str());
	printf("cc: %s\n", cc.c_str());
	printf("cxx: %s\n", cxx.c_str());
	printf("arch: ");
	PrintList(arch);
	printf("dist: %s\n", dist.c_str());
	printf("include: %s\n", include.c_str());
	printf("libs: ");
	PrintList(libs);
	printf("incdir: ");
	PrintList(incdir);
	printf("libdir: ");
	PrintList(libdir);
	printf("remote: %s\n", remote.c_str());
	printf("defines: %s\n", defines.c_str());
	printf("before-script: ");
	PrintList(before);
	printf("after-script: ");
	PrintList(after);
}

void Profile::PopValidValue(string k_value, string v_value)
{
	if (strcasecmp("os", k_value.c_str()) == 0) {
		os = v_value;
	}
	if (strcasecmp("arch", k_value.c_str()) == 0) {
		arch.push_back(v_value);
	}
	if (strcasecmp("cc", k_value.c_str()) == 0) {
		cc = v_value;
	}
	if (strcasecmp("cxx", k_value.c_str()) == 0) {
		cxx = v_value.c_str();
	}
	if (strcasecmp("include", k_value.c_str()) == 0) {
		include = v_value;
	}
	if (strcasecmp("lang", k_value.c_str()) == 0) {
		lang = v_value;
	}
	if (strcasecmp("dist", k_value.c_str()) == 0) {
		dist = v_value;
	}
	if (strcasecmp("before-script", k_value.c_str()) == 0) {
		before.push_back(v_value);
	}
	if (strcasecmp("after-script", k_value.c_str()) == 0) {
		after.push_back(v_value);
	}
	if (strcasecmp("libs", k_value.c_str()) == 0) {
		libs.push_back(PrependLink(v_value, "-l"));
	}
	if (strcasecmp("incdir", k_value.c_str()) == 0) {
		incdir.push_back(PrependLink(v_value, "-I"));
	}
	if (strcasecmp("libdir", k_value.c_str()) == 0) {
		libdir.push_back(PrependLink(v_value, "-L"));
	}
	if (strcasecmp("remote", k_value.c_str()) == 0) {
		remote = v_value;
	}
	if (strcasecmp("defines", k_value.c_str()) == 0) {
		defines = v_value;
	}
}
