// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the copyright holder nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIA DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

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
#include "gen.h"
#include "platdef.h"
#define BASEDIR DefineBaseDir()

enum {
	FS_OK = 0,
	FS_BADPattern,
	FS_NAMETOOLONG,
	FS_BADIO,
};

Generate::Generate() {};
Generate::~Generate() {};

char *Generate::DefineBaseDir()
{
	return current_dir;
}

int Generate::WalkRecur(const char *dir_name, regex_t *expr, int spec)
{
	struct dirent *ent;
	DIR *dir;
	char path_name[FILENAME_MAX];
	int res = FS_OK;
	int len = strlen(dir_name);
	if (len >= FILENAME_MAX - 1)
		return FS_NAMETOOLONG;
	strcpy(path_name, dir_name);
	path_name[len++] = '/';
	if (!(dir = opendir(dir_name))) {
		printf("Error: Can't open %s", dir_name);
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
		if (!regexec(expr, path_name, 0, 0, 0))
			printf("%s\n", path_name);
	}
#endif
	if (dir)
		closedir(dir);
	return res ? res : errno ? FS_BADIO : FS_OK;
}

int Generate::WalkDir(const char *dir_name, const char *pattern, int spec)
{
	regex_t r;
	int res;
	if (regcomp(&r, pattern, REG_EXTENDED | REG_NOSUB))
		return FS_BADPattern;
	res = WalkRecur(dir_name, &r, spec);
	regfree(&r);

	return res;
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
	snprintf(file_name, sizeof(file_name), "%s.ybf", basename(BASEDIR));
	if (access(file_name, F_OK) != -1) {
		return 1;
	} else {
		return -1;
	}
	return 0;
}

void Generate::GenBlankConfig(int force_opt)
{
	char file_name[PATH_MAX];
	if ((CheckConfigExists() < 0) && (force_opt == 0)) {
		snprintf(file_name, sizeof(file_name), "%s.ybf", basename(BASEDIR));
		printf("New build file written as: %s\n", file_name);
		new_config = fopen(file_name, "w+");
	} else if (CheckConfigExists() > 0) {
		snprintf(file_name, sizeof(file_name), "%s.ybf", basename(BASEDIR));
		printf("Config file %s already exists\n", file_name);
		if (force_opt > 0) {
			snprintf(file_name, sizeof(file_name), "%s.ybf", basename(BASEDIR));
			printf("New build file written as: %s\n", file_name);
			new_config = fopen(file_name, "w+");
		}
	}
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
		std::cout << std::setfill('#') << std::setw(80) << "#" << std::endl;
		std::cout << std::setfill('#') << std::setw(2) << "#"
			<< "\t\t\tMakefile Generated with yabs" << std::endl;
		std::cout << std::setfill('#') << std::setw(80) << "#" << std::endl;
		return 1;
	} else {
		return -1;
	}
}
