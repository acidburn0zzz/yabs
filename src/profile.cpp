// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <string.h>
#include <yaml.h>
#include <algorithm>
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

void Profile::GetSysInfo()
{
#ifdef __linux__
	plat = "linux";
	if (cc.empty())
		cc = "gcc";
	if (cxx.empty())
		cxx = "g++";
#elif __FreeBSD__
	plat = "freebsd";
	if (cc.empty())
		cc = "clang";
	if (cxx.empty())
		cxx = "clang++";
#endif
#ifdef __amd64__
	p_arch = "x86_64";
#endif
#ifdef __i386__
	p_arch = "i686";
#endif
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

void Profile::BuildObjList()
{
	obj = FileList;
	for (int i = 0; i < (int)obj.size(); i++) {
		obj[i] = strrchr(&obj[i][0], '/') + 1;
		size_t f = obj[i].find(lang);
		obj[i].replace(f, lang.length(), "o");
	}
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

std::string Profile::GetOS() const { return this->os; }

void Profile::PrintProfile()
{
	printf("target: %s\n", target.c_str());
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
	printf("remote: %s\n", remote.c_str());
	printf("defines: %s\n", defines.c_str());
	printf("before-script: ");
	PrintList(before);
	printf("after-script: ");
	PrintList(after);
}

void Profile::CheckBlankValues()
{
	GetSysInfo();
	if (os.empty()) {
		os = plat;
	}
	if (std::find(arch.begin(), arch.end(), p_arch) == arch.end()) {
		arch.push_back(p_arch);
	}
	if (incdir.empty()) {
		incdir.push_back("-I/usr/include");
		incdir.push_back("-I/usr/local/include");
	}
	if (target.empty()) {
		target = GetRelBase();
	}
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
		cxx = v_value;
	}
	if (strcasecmp("target", k_value.c_str()) == 0) {
		target = v_value;
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
	if (strcasecmp("remote", k_value.c_str()) == 0) {
		remote = v_value;
	}
	if (strcasecmp("defines", k_value.c_str()) == 0) {
		defines = v_value;
	}
	if (strcasecmp("cxxflags", k_value.c_str()) == 0) {
		cxxflags.push_back(PrependLink(v_value, "-"));
	}
}

void Profile::CheckLang()
{
	if (lang == "c") {
		WalkDir(GetCurrentDir(), ".\\.c$", FS_DEFAULT | FS_MATCHDIRS);
	} else if (lang == "cpp") {
		WalkDir(GetCurrentDir(), ".\\.cpp$", FS_DEFAULT | FS_MATCHDIRS);
	}
}

int Profile::WriteMake(const char *makefile)
{
	CheckLang();
	CheckBlankValues();
	Makefile = fopen(makefile, "w+");
	fprintf(Makefile, "TRGT\t= %s\n", target.c_str());
	fprintf(Makefile, "LINK\t= %s\n", cxx.c_str());
	fprintf(Makefile, "CC\t= %s\n", cc.c_str());
	fprintf(Makefile, "CXX\t= %s\n", cxx.c_str());

	for (int i = 0; i < (int)cxxflags.size(); i++) {
		temp += cxxflags[i] += " ";
	}
	fprintf(Makefile, "CXXFLAGS= %s\n", temp.c_str());
	temp.clear();

	for (int i = 0; i < (int)libs.size(); i++) {
		temp += libs[i] += " ";
	}
	fprintf(Makefile, "LIBS\t= %s\n", temp.c_str());
	temp.clear();

	for (int i = 0; i < (int)incdir.size(); i++) {
		temp += incdir[i] += " ";
	}
	fprintf(Makefile, "INCPATH\t= %s\n", temp.c_str());
	temp.clear();

	fprintf(Makefile, "SRC\t=");
	for (unsigned int i = 0; i < FileList.size(); i++) {
		if (i == 0) {
			fprintf(Makefile, " %s \\\n", FileList[i].c_str());
		} else {
			if (i == (FileList.size() - 1)) {
				fprintf(Makefile, "\t  %s\n",
					FileList[i].c_str());
			} else {
				fprintf(Makefile, "\t  %s \\\n",
					FileList[i].c_str());
			}
		}
	}

	BuildObjList();
	fprintf(Makefile, "OBJ\t=");
	for (unsigned int i = 0; i < obj.size(); i++) {
		if (i == 0) {
			fprintf(Makefile, " %s \\\n", obj[i].c_str());
		} else {
			if (i == (obj.size() - 1)) {
				fprintf(Makefile, "\t  %s\n", obj[i].c_str());
			} else {
				fprintf(Makefile, "\t  %s \\\n",
					obj[i].c_str());
			}
		}
	}
	fprintf(Makefile, "DEL\t= rm -f\n");
	fprintf(Makefile, "\n.SUFFIXES: .o .c .cpp .cc .cxx .C\n\n");
	fprintf(
	    Makefile,
	    ".cpp.o:\n\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".cc.o:\n\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".cxx.o:\n\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".C.o:\n\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".c.o:\n\t$(CC) -c $(CXXFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");

	fprintf(Makefile, "all: $(TRGT)\n\n");
	fprintf(Makefile, "$(TRGT): $(OBJ)\n\t$(CXX) $(LFLAGS) -o $(TRGT) "
			  "$(OBJ) $(LIBS)\n\n");

	for (int i = 0; i < (int)obj.size(); i++) {
		fprintf(Makefile, "%s: %s\n", obj[i].c_str(),
			FileList[i].c_str());
		fprintf(Makefile,
			"\t$(CXX) -c $(CXXFLAGS) $(INCPATH) -o %s %s\n\n",
			obj[i].c_str(), FileList[i].c_str());
	}
	fprintf(Makefile, "clean:\n\t$(DEL) $(OBJ)\n\t$(DEL) $(TRGT)\n");

	fclose(Makefile);
	return 0;
}
