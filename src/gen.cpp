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

Generate::Generate()
{
	bin_num = 0;
	file_count = 0;
	current_dir = BASEDIR;
}

Generate::~Generate(){};

char *Generate::GetCurrentDir() { return current_dir; }

std::string Generate::GetRelBase() { return REL_BASEDIR; }

int Generate::WalkRecur(std::string dir_name, regex_t *expr, int spec)
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
			FileList.push_back(RelPathName(file_name));
		}
	}
#endif
	if (dir)
		closedir(dir);
	return res ? res : errno ? FS_BADIO : FS_OK;
}

int Generate::SearchForMain(const std::vector<std::string> &vect)
{
	for (int i = 0; i < (int)vect.size(); i++) {
		if ((src_file = fopen(vect[i].c_str(), "r")) != NULL) {
			while (fgets(temp, 512, src_file) != NULL) {
				if (strstr(temp, ENTRY_P) != NULL) {
					++bin_num;
				}
			}
			fclose(src_file);
		}
	}
	return bin_num;
}

std::string Generate::ParseLang(std::string ext)
{
	std::string reg_pre = ".\\.";
	std::string reg_app = "$";
	reg_pre += ext;
	reg_pre += reg_app;
	return reg_pre;
}

int Generate::WalkDir(std::string dir_name, std::string pattern, int spec)
{
	regex_t r;
	int res;
	pattern = ParseLang(pattern);
	if (regcomp(&r, pattern.c_str(), REG_EXTENDED | REG_NOSUB))
		return FS_BADPattern;
	res = WalkRecur(dir_name, &r, spec);
	regfree(&r);

	return res;
}

std::string Generate::RelPathName(std::string &to_rel)
{
	rm_base = REL_BASEDIR;
	std::string perm = strstr(&to_rel[0], rm_base.c_str());
	perm.erase(0, rm_base.length() + 1);
	return perm;
}

void Generate::PrintFileList()
{
	for (int i = 0; i < file_count; i++) {
		printf("File list: %s\n", FileList[i].c_str());
	}
}

int Generate::CheckMake()
{
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

int Generate::GenConfig(int force_opt)
{
	char file_name[PATH_MAX];
	if ((CheckConfigExists() < 0) && (force_opt == 0)) {
		snprintf(file_name, sizeof(file_name), "%s.ybf",
			 basename(BASEDIR));
		new_config = fopen(file_name, "w+");
		if (new_config != NULL) {
			fprintf(new_config, DOC_START);
			fprintf(new_config, DOC_END);
			printf("New build file written as: %s\n", file_name);
			fclose(new_config);
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
		fprintf(new_config, DOC_START);
		fprintf(new_config, DOC_END);
		new_config = fopen(file_name, "w+");
		return 3;
	}
	return 3;
}
