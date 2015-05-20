// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "profile.h"

Profile::Profile() {}

std::string Profile::ConvValue(unsigned char *conv_value)
{
	std::string temp_value;
	temp_value.append(reinterpret_cast<const char *>(conv_value));
	return temp_value;
}

std::string Profile::PrependLink(std::string &to_proc, std::string pre)
{
	std::string space = " ";
	to_proc = pre + to_proc;
	pre = space + pre;
	size_t start_pos = 0;
	while ((start_pos = to_proc.find(space, start_pos)) !=
	       std::string::npos) {
		to_proc.replace(start_pos, space.length(), pre);
		start_pos += pre.length();
	}
	return to_proc;
}

std::string Profile::GetOS() const
{
	return this->ProfileMap.find("os")->second;
}

std::string Profile::MapValuesToString(std::string key)
{
	temp.clear();
	auto range = ProfileMap.equal_range(key);
	for (ProfileIter it = range.first; it != range.second; ++it) {
		temp.append(it->second + " ");
	}
	return temp;
}

std::string Profile::VectToString(std::vector<std::string> &vect)
{
	temp.clear();
	for (unsigned i = 0; i < vect.size(); ++i) {
		temp += vect[i] += " ";
	}
	return temp;
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

int Profile::WriteMake(const char *makefile)
{
	CheckLang();
	IgnorePath(FileList);
	SrcList();
	CheckBlankValues();
	Makefile = fopen(makefile, "w+");
	fprintf(Makefile, "INSTALL\t= /usr/bin/env install\n"
			  "DEST\t=\n"
			  "PREFIX\t= %s\n"
			  "BINDIR\t= $(PREFIX)/bin\n"
			  "LIBDIR\t= $(PREFIX)/lib\n"
			  "MANDIR\t= $(PREFIX)/share/man\n",
		MapValuesToString("install").c_str());
	fprintf(Makefile, "TRGT\t= %s\n"
			  "LINK\t= %s\n"
			  "COMP\t= %s\n",
		MapValuesToString("target").c_str(),
		MapValuesToString("comp").c_str(),
		MapValuesToString("comp").c_str());

	WriteMacroValues(MapValuesToString("cflags"), "CFLAGS");
	WriteMacroValues(MapValuesToString("libs"), "LIBS");
	WriteMacroValues(MapValuesToString("incdir"), "INCPATH");
	WriteMacroValues(MapValuesToString("libdir"), "LIBDIR");

	fprintf(Makefile, "CLN\t= ");
	if (ProfileMap.count("clean") == 0) {
		fprintf(Makefile, "\n");
	} else {
		auto range = ProfileMap.equal_range("clean");
		auto range_end = --ProfileMap.upper_bound("clean");
		auto range_begin = ProfileMap.lower_bound("clean");
		for (ProfileIter it = range.first; it != range.second; ++it) {
			if (it == range_begin) {
				fprintf(Makefile, "%s \\\n",
					it->second.c_str());
			} else if (it == range_end) {
				fprintf(Makefile, "\t  %s\n",
					it->second.c_str());
			} else {
				fprintf(Makefile, "\t  %s \\\n",
					it->second.c_str());
			}
		}
	}

	BuildObjList();
	WriteVecValues(obj, "OBJ");
	fprintf(Makefile, "DEL\t= rm -f\n");
	fprintf(Makefile, "\n.SUFFIXES: .o .c .cpp .cc .cxx .C\n\n");
	fprintf(
	    Makefile,
	    ".cpp.o:\n\t$(COMP) -c $(CFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".cc.o:\n\t$(COMP) -c $(CFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".cxx.o:\n\t$(COMP) -c $(CFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".C.o:\n\t$(COMP) -c $(CFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");
	fprintf(
	    Makefile,
	    ".c.o:\n\t$(COMP) -c $(CFLAGS) $(INCPATH) -o \"$@\" \"$<\"\n\n");

	fprintf(Makefile, "all: $(TRGT)\n\n");
	fprintf(Makefile, "$(TRGT): $(OBJ)\n\t$(COMP) $(LFLAGS) -o $(TRGT) "
			  "$(OBJ) $(LIBDIR) $(LIBS)\n\n");

	for (unsigned i = 0; i < obj.size(); i++) {
		fprintf(Makefile, "%s: %s\n", obj[i].c_str(),
			FileList[i].c_str());
		fprintf(Makefile,
			"\t$(COMP) -c $(CFLAGS) $(INCPATH) -o %s %s\n\n",
			obj[i].c_str(), FileList[i].c_str());
	}
	fprintf(Makefile,
		"clean:\n\t$(DEL) $(OBJ)\n\t$(DEL) $(CLN)\n\t$(DEL) $(TRGT)\n");

	fclose(Makefile);
	return 0;
}

int Profile::Build()
{
	ExecScript("before-script");
	CheckLang();
	IgnorePath(FileList);
	SrcList();
	CheckBlankValues();
	BuildObjList();
	std::string cmd_str;
	std::string temp_comp =
	    MapValuesToString("cflags") + MapValuesToString("incdir");
	for (unsigned i = 0; i < FileList.size(); i++) {
		cmd_str = MapValuesToString("comp") + " -c " + temp_comp +
			  "-o " + obj[i] + " " + FileList[i];
		printf("%s\n", cmd_str.c_str());
		system(cmd_str.c_str());
		cmd_str.clear();
	}
	cmd_str = MapValuesToString("comp") + " -o " +
		  MapValuesToString("target") + " " + VectToString(obj) +
		  MapValuesToString("libdir") + MapValuesToString("libs");
	printf("%s\n", cmd_str.c_str());
	system(cmd_str.c_str());
	ExecScript("after-script");
	return 0;
}

void Profile::ExecScript(std::string script_list)
{
	auto range = ProfileMap.equal_range(script_list);
	for (ProfileIter it = range.first; it != range.second; ++it) {
		printf("%s\n", it->second.c_str());
		system(it->second.c_str());
	}
}

void Profile::OpenInclude(const std::string file)
{
	inc_conf = fopen(file.c_str(), "r");
	if (inc_conf == NULL) {
		printf("Error: Couldn't open included file: %s\n",
		       file.c_str());
	}
}

void Profile::PopValidValue(std::string &k_value, std::string v_value)
{
	for (int i = 0; i < MAX_OPT; i++) {
		if (strcasecmp(STDValues[i].c_str(), k_value.c_str()) == 0) {
			if (STDValues[i] == "cflags") {
				v_value = PrependLink(v_value, "-");
			} else if (STDValues[i] == "lflags") {
				v_value = PrependLink(v_value, "-");
			} else if (STDValues[i] == "libdir") {
				v_value = PrependLink(v_value, "-L");
			} else if (STDValues[i] == "incdir") {
				v_value = PrependLink(v_value, "-I");
			} else if (STDValues[i] == "libs") {
				v_value = PrependLink(v_value, "-l");
			}
			ProfileMap.insert(std::pair<std::string, std::string>(
			    k_value, v_value));
		}
	}
}

void Profile::SrcList()
{
	if (ProfileMap.count("src") != 0) {
		FileList.clear();
		auto range = ProfileMap.equal_range("src");
		for (ProfileIter it = range.first; it != range.second; ++it) {
			FileList.push_back(it->second);
		}
	}
}

void Profile::IgnorePath(std::vector<std::string> &vect)
{
	if (ProfileMap.count("ignore") != 0) {
		auto range = ProfileMap.equal_range("ignore");
		for (ProfileIter it = range.first; it != range.second; ++it) {
			vect.erase(std::remove_if(vect.begin(), vect.end(),
						  [&](std::string s) {
					   return s.find(it->second) !=
						  std::string::npos;
				   }),
				   vect.end());
		}
	}
}

void Profile::BuildObjList()
{
	obj = FileList;
	for (unsigned i = 0; i < obj.size(); i++) {
		if (obj[i].c_str() != NULL) {
			size_t ext = obj[i].rfind('.') + 1;
			obj[i].replace(ext, sizeof(ext), "o");
		}
		if (strchr(&obj[i][0], '/') != NULL) {
			obj[i] = strrchr(&obj[i][0], '/') + 1;
		}
	}
}

void Profile::PrintProfile() const
{
	for (auto it = ProfileMap.cbegin(); it != ProfileMap.cend(); it++) {
		std::cout << it->first << " => " << it->second << '\n';
	}
}

void Profile::CheckBlankValues()
{
#ifdef __linux__
	if (ProfileMap.count("os") == 0)
		ProfileMap.insert(
		    std::pair<std::string, std::string>("os", "linux"));
	if (ProfileMap.count("comp") == 0)
		ProfileMap.insert(
		    std::pair<std::string, std::string>("comp", "gcc"));
#elif __FreeBSD__
	if (ProfileMap.count("os") == 0)
		ProfileMap.insert(
		    std::pair<std::string, std::string>("os", "freebsd"));
	if (ProfileMap.count("comp") == 0)
		ProfileMap.insert(
		    std::pair<std::string, std::string>("comp", "clang"));
#endif
#ifdef __amd64__
	if (ProfileMap.count("arch") == 0)
		ProfileMap.insert(
		    std::pair<std::string, std::string>("arch", "x86_64"));
#endif
#ifdef __i386__
	if (ProfileMap.count("arch") == 0)
		ProfileMap.insert(
		    std::pair<std::string, std::string>("arch", "i686"));
#endif
	if (ProfileMap.count("incdir") == 0) {
		ProfileMap.insert(std::pair<std::string, std::string>(
		    "incdir", "-I/usr/include"));
	}
	if (ProfileMap.count("libdir") == 0) {
		ProfileMap.insert(std::pair<std::string, std::string>(
		    "libdir", "-L/usr/lib"));
	}
	if (ProfileMap.count("target") == 0) {
		ProfileMap.insert(std::pair<std::string, std::string>(
		    "target", GetRelBase()));
	}
	if (ProfileMap.count("install") == 0) {
		ProfileMap.insert(std::pair<std::string, std::string>(
		    "install", "/usr/local"));
	}
}

void Profile::WriteVecValues(std::vector<std::string> &vect,
			     std::string out_name)
{
	fprintf(Makefile, "%s\t=", out_name.c_str());
	if (vect.size() == 0) {
		fprintf(Makefile, "\n");
	} else {
		for (unsigned i = 0; i < vect.size(); i++) {
			if (vect.size() == 1) {
				fprintf(Makefile, " %s\n", vect[i].c_str());
				break;
			}
			if (i == 0) {
				fprintf(Makefile, " %s \\\n", vect[i].c_str());
			} else {
				if (i == (vect.size() - 1)) {
					fprintf(Makefile, "\t  %s\n",
						vect[i].c_str());
				} else {
					fprintf(Makefile, "\t  %s \\\n",
						vect[i].c_str());
				}
			}
		}
	}
}

void Profile::WriteMacroValues(const std::string &val, std::string out_name)
{
	fprintf(Makefile, "%s\t= %s\n", out_name.c_str(), val.c_str());
}

void Profile::CheckLang()
{
	ProfileConstIter it = ProfileMap.find("lang");
	WalkDir(GetCurrentDir(), it->second, FS_DEFAULT | FS_MATCHDIRS);
}
