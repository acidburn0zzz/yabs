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

std::string Profile::GetOS() const { return this->os; }

std::string Profile::VectToString(std::vector<std::string> &to_swap)
{
	temp.clear();
	for (unsigned i = 0; i < to_swap.size(); i++) {
		temp += to_swap[i] += " ";
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
	CleanList(FileList);
	SrcList();
	CheckBlankValues();
	Makefile = fopen(makefile, "w+");
	fprintf(Makefile, "TRGT\t= %s\n", target.c_str());
	fprintf(Makefile, "LINK\t= %s\n", comp.c_str());
	fprintf(Makefile, "COMP\t= %s\n", comp.c_str());

	WriteSwapValues(VectToString(cflags), "CFLAGS");
	WriteSwapValues(VectToString(libs), "LIBS");
	WriteSwapValues(VectToString(incdir), "INCPATH");
	WriteSwapValues(VectToString(libdir), "LIBDIR");

	WriteListToMake(clean, "CLN");
	BuildObjList();
	WriteListToMake(obj, "OBJ");

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
	CheckLang();
	CleanList(FileList);
	SrcList();
	CheckBlankValues();
	BuildObjList();
	std::string cmd_str;
	std::string temp_comp = VectToString(cflags) + VectToString(incdir);
	for (unsigned i = 0; i < FileList.size(); i++) {
		cmd_str = comp + " -c " + temp_comp + "-o " + obj[i] + " " +
			  FileList[i];
		printf("%s\n", cmd_str.c_str());
		system(cmd_str.c_str());
	}
	cmd_str = comp + " -o " + target + " " + VectToString(obj) +
		  VectToString(libdir) + VectToString(libs);
	printf("%s\n", cmd_str.c_str());
	system(cmd_str.c_str());
	return 0;
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
	if (strcasecmp("os", k_value.c_str()) == 0) {
		os = v_value;
		return;
	} else if (strcasecmp("version", k_value.c_str()) == 0) {
		version = v_value;
		return;
	} else if (strcasecmp("arch", k_value.c_str()) == 0) {
		arch.push_back(v_value);
		return;
	} else if (strcasecmp("ignore", k_value.c_str()) == 0) {
		ignore.push_back(v_value);
		return;
	} else if (strcasecmp("src", k_value.c_str()) == 0) {
		src.push_back(v_value);
		return;
	} else if (strcasecmp("comp", k_value.c_str()) == 0) {
		comp = v_value;
		return;
	} else if (strcasecmp("cflags", k_value.c_str()) == 0) {
		cflags.push_back(PrependLink(v_value, "-"));
		return;
	} else if (strcasecmp("target", k_value.c_str()) == 0) {
		target = v_value;
		return;
	} else if (strcasecmp("include", k_value.c_str()) == 0) {
		include = v_value;
		return;
	} else if (strcasecmp("lang", k_value.c_str()) == 0) {
		lang = v_value;
		return;
	} else if (strcasecmp("dist", k_value.c_str()) == 0) {
		dist = v_value;
		return;
	} else if (strcasecmp("before-script", k_value.c_str()) == 0) {
		before.push_back(v_value);
		return;
	} else if (strcasecmp("after-script", k_value.c_str()) == 0) {
		after.push_back(v_value);
		return;
	} else if (strcasecmp("libs", k_value.c_str()) == 0) {
		libs.push_back(PrependLink(v_value, "-l"));
		return;
	} else if (strcasecmp("incdir", k_value.c_str()) == 0) {
		incdir.push_back(PrependLink(v_value, "-I"));
		return;
	} else if (strcasecmp("libdir", k_value.c_str()) == 0) {
		libdir.push_back(PrependLink(v_value, "-L"));
		return;
	} else if (strcasecmp("remote", k_value.c_str()) == 0) {
		remote = v_value;
		return;
	} else if (strcasecmp("defines", k_value.c_str()) == 0) {
		defines = v_value;
		return;
	} else if (strcasecmp("clean", k_value.c_str()) == 0) {
		clean.push_back(v_value);
		return;
	}
}

void Profile::PrintList(const std::vector<std::string> vect) const
{
	if (vect.size() == 0) {
		printf("\tNone\n");
	} else {
		for (unsigned int i = 0; i < vect.size(); i++) {
			printf("\t%s\n", vect[i].c_str());
		}
	}
}

void Profile::SrcList()
{
	if (!src.empty()) {
		FileList.clear();
		for (unsigned i = 0; i < src.size(); i++) {
			FileList.push_back(src[i]);
		}
	}
}

void Profile::GetSysInfo()
{
#ifdef __linux__
	plat = "linux";
	if (comp.empty())
		comp = "g++";
#elif __FreeBSD__
	plat = "freebsd";
	if (comp.empty())
		comp = "clang++";
#endif
#ifdef __amd64__
	p_arch = "x86_64";
#endif
#ifdef __i386__
	p_arch = "i686";
#endif
}

void Profile::CleanList(std::vector<std::string> &vect)
{
	for (unsigned i = 0; i < ignore.size(); i++) {
		vect.erase(std::remove_if(vect.begin(), vect.end(),
					  [&](std::string s) {
				   return s.find(ignore[i]) !=
					  std::string::npos;
			   }),
			   vect.end());
	}
}

void Profile::BuildObjList()
{
	obj = FileList;
	for (unsigned i = 0; i < obj.size(); i++) {
		if (obj[i].c_str() != NULL) {
			size_t f = obj[i].find(lang);
			obj[i].replace(f, lang.length(), "o");
		}
		if (strchr(&obj[i][0], '/') != NULL) {
			obj[i] = strrchr(&obj[i][0], '/') + 1;
		}
	}
}

void Profile::PrintProfile() const
{
	printf("target:\n\t%s\n", target.c_str());
	printf("version:\n\t%s\n", version.c_str());
	printf("os:\n\t%s\n", os.c_str());
	printf("comp:\n\t%s\n", comp.c_str());
	printf("cflags:\n");
	PrintList(cflags);
	printf("arch:\n");
	PrintList(arch);
	printf("dist:\n\t%s\n", dist.c_str());
	printf("src:\n");
	PrintList(src);
	printf("ignore:\n");
	PrintList(ignore);
	printf("include:\n\t%s\n", include.c_str());
	printf("libs:\n");
	PrintList(libs);
	printf("incdir:\n");
	PrintList(incdir);
	printf("libdir:\n");
	PrintList(libdir);
	printf("remote:\n\t%s\n", remote.c_str());
	printf("defines:\n\t%s\n", defines.c_str());
	printf("before-script:\n");
	PrintList(before);
	printf("after-script:\n");
	PrintList(after);
	printf("clean:\n");
	PrintList(clean);
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
	if (libdir.empty()) {
		libdir.push_back("-L/usr/lib");
		libdir.push_back("-L/usr/local/lib");
	}
	if (target.empty()) {
		target = GetRelBase();
	}
}

void Profile::WriteListToMake(std::vector<std::string> &vect,
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

void Profile::WriteSwapValues(const std::string &val, std::string out_name)
{
	fprintf(Makefile, "%s\t= %s\n", out_name.c_str(), val.c_str());
}

void Profile::CheckLang()
{
	WalkDir(GetCurrentDir(), lang, FS_DEFAULT | FS_MATCHDIRS);
}
