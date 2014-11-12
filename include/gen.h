// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _GEN_H
#define _GEN_H

#define MAKEFILE "Makefile"
#define FS_NONE 0
#define FS_RECURSIVE (1 << 0)
#define FS_DEFAULT FS_RECURSIVE
#define FS_FOLLOWLINK (1 << 1)
#define FS_DOTFILES (1 << 2)
#define FS_MATCHDIRS (1 << 3)

#include <regex.h>
#include <unistd.h>
#include <sys/param.h>
#include <fstream>
#include <string>
#include <vector>

enum { FS_OK = 0,
       FS_BADPattern,
       FS_NAMETOOLONG,
       FS_BADIO,
};

class Generate
{
private:
	char cwd[MAXPATHLEN];
	char *current_dir = getcwd(cwd, MAXPATHLEN);
	std::vector<std::string> FileList;
	char *file_name;
	const char *default_makefile;
	int file_count = 0;
	FILE *makefile;
	FILE *new_config;

public:
	Generate();
	~Generate();
	char *GetCurrentDir();
	char *RelPathName(char *to_rel);
	int WriteMake();
	void Walk();
	void GenFileList(char *file_list);
	int GenBlankConfig(int force_opt);
	void CheckFiles();
	void PrintFileList();
	int CheckConfigExists();
	int CheckMake();
	int GenMakeFromTemplate();
	int WalkDir(std::string dir_name, std::string pattern, int spec);
	int WalkRecur(std::string dir_name, regex_t *expr, int spec);
};
#endif
