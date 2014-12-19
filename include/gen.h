// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _GEN_H
#define _GEN_H

#define MAKEFILE "Makefile"
#define ENTRY_P "int main"
#define FS_NONE 0
#define FS_RECURSIVE (1 << 0)
#define FS_DEFAULT FS_RECURSIVE
#define FS_FOLLOWLINK (1 << 1)
#define FS_DOTFILES (1 << 2)
#define FS_MATCHDIRS (1 << 3)
#define DOC_START "---\n"
#define DOC_END "...\n"

#include <regex.h>
#include <unistd.h>
#include <sys/param.h>
#include <fstream>
#include <string>
#include <vector>

class Generate
{
private:
	enum { FS_OK = 0,
	       FS_BADPattern,
	       FS_NAMETOOLONG,
	       FS_BADIO,
	};
	char cwd[MAXPATHLEN];
	char temp[512];
	char *current_dir;
	const char *default_makefile;
	std::string file_name;
	std::string rm_base;
	int file_count;
	int bin_num;
	FILE *makefile;
	FILE *new_config;
	FILE *src_file;

public:
	Generate();
	~Generate();
	std::vector<std::string> FileList;
	std::string RelPathName(std::string to_rel);
	std::string GetRelBase();
	char *GetCurrentDir();
	char *RemoveBase(char *to_rm);
	void Walk();
	void GenFileList(std::string file_list);
	void CheckFiles();
	void PrintFileList();
	int SearchForMain(std::vector<std::string> vect);
	int GenConfig(int force_opt);
	int CheckConfigExists();
	int CheckMake();
	int GenMakeFromTemplate();
	int WalkDir(std::string dir_name, std::string pattern, int spec);
	int WalkRecur(std::string dir_name, regex_t *expr, int spec);
};

#endif
