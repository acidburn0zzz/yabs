// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifdef __linux__
#include <linux/limits.h>
#include <err.h>
#endif
#include <errno.h>
#include <dirent.h>
#include <libgen.h>
#include <regex.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "gen.h"
#define BASEDIR GetCurrentDir()
#define REL_BASEDIR strrchr(BASEDIR, '/') + 1

using std::string;

Generate::Generate(){};
Generate::~Generate(){};

char *Generate::GetCurrentDir() { return current_dir; }

int Generate::WalkRecur(string dir_name, regex_t *expr, int spec)
{
	struct dirent *ent;
	DIR *dir;
	char path_name[FILENAME_MAX];
	int res = FS_OK;
	int len = strlen(dir_name.c_str());
	if (len >= FILENAME_MAX - 1)
		return FS_NAMETOOLONG;
	strcpy(path_name, dir_name.c_str());
	path_name[len++] = '/';
	if (!(dir = opendir(dir_name.c_str()))) {
		printf("Error: Can't open %s", dir_name.c_str());
		return FS_BADIO;
	}
	errno = 0;
#ifdef __unix__
	struct stat St;
	while ((ent = readdir(dir))) {
		if (!(spec & FS_DOTFILES) && ent->d_name[0] == '.')
			continue;
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
			continue;
		strncpy(path_name + len, ent->d_name, FILENAME_MAX - len);
		if (lstat(path_name, &St) == -1) {
			printf("Error: Can't stat %s", path_name);
			res = FS_BADIO;
			continue;
		}
		if (S_ISLNK(St.st_mode) && !(spec & FS_FOLLOWLINK))
			continue;
		if (S_ISDIR(St.st_mode)) {
			if ((spec & FS_RECURSIVE))
				WalkRecur(path_name, expr, spec);
			if (!(spec & FS_MATCHDIRS))
				continue;
		}
		if (!regexec(expr, path_name, 0, 0, 0)) {
			file_count++;
			file_name = path_name;
			GenFileList(RelPathName(file_name));
		}
	}
#endif
	if (dir)
		closedir(dir);
	return res ? res : errno ? FS_BADIO : FS_OK;
}

int Generate::WalkDir(string dir_name, string pattern, int spec)
{
	regex_t r;
	int res;
	if (regcomp(&r, pattern.c_str(), REG_EXTENDED | REG_NOSUB))
		return FS_BADPattern;
	res = WalkRecur(dir_name, &r, spec);
	regfree(&r);

	return res;
}

char *Generate::RelPathName(char *to_rel)
{
	char *perm = strstr(to_rel, REL_BASEDIR);
	return perm;
}

void Generate::GenFileList(char *file_list)
{
	if (file_list != NULL) {
		std::string fileList = file_list;
		FileList.push_back(fileList);
	}
}

void Generate::PrintFileList()
{
	for (int i = 0; i < file_count; i++) {
		std::cout << "Vector file list: " << FileList[i] << "\n";
	}
}

int Generate::CheckMake()
{
	// Get current working directory
	if (getcwd(cwd, MAXPATHLEN) != NULL) {
		printf("Current working directory: %s\n", cwd);
		struct stat buffer;
		int exist = stat(MAKEFILE, &buffer);
		if (exist == 0) {
			printf("Makefile present\n");
			return 1;
		} else {
			printf("No Makefile present\n");
			return -1;
		}
	}
	return 0;
}

int Generate::CheckConfigExists()
{
	char file_name[PATH_MAX];
	snprintf(file_name, sizeof(file_name), "%s.ybf", basename(REL_BASEDIR));
	if (access(file_name, F_OK) != -1) {
		return 1;
	} else {
		return -1;
	}
	return 0;
}

int Generate::GenBlankConfig(int force_opt)
{
	char file_name[PATH_MAX];
	if ((CheckConfigExists() < 0) && (force_opt == 0)) {
		snprintf(file_name, sizeof(file_name), "%s.ybf",
			 basename(BASEDIR));
		new_config = fopen(file_name, "w+");
		if (new_config != NULL) {
			printf("New build file written as: %s\n", file_name);
		}
		return 1;
	} else if (CheckConfigExists() > 0) {
		snprintf(file_name, sizeof(file_name), "%s.ybf",
			 basename(BASEDIR));
		printf("Config file %s already exists\n", file_name);
		return 2;
	}
	if (force_opt > 0) {
		snprintf(file_name, sizeof(file_name), "%s.ybf",
			 basename(BASEDIR));
		printf("New build file written as: %s\n", file_name);
		new_config = fopen(file_name, "w+");
		return 3;
	}
	return 3;
}

int Generate::WriteMake()
{
	if (CheckConfigExists() == 1) {
		return 1;
	} else {
		printf("Error: yabs build file does not exist\n");
		return -1;
	}
	return 0;
}

int Generate::GenMakeFromTemplate()
{
	if (CheckMake() != 1) {
		std::cout << std::setfill('#') << std::setw(80) << "#"
			  << std::endl;
		std::cout << std::setfill('#') << std::setw(2) << "#"
			  << "\t\t\tMakefile Generated with yabs" << std::endl;
		std::cout << std::setfill('#') << std::setw(80) << "#"
			  << std::endl;
		return 1;
	} else {
		return -1;
	}
}
