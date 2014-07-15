#include "gen.h"
#ifdef __linux__
#include <linux/limits.h>
#endif
#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#define BASEDIR DefineBaseDir()

Generate::Generate() {};
Generate::~Generate() {};

char *Generate::DefineBaseDir()
{
	char *currentDir = get_current_dir_name();
	return currentDir;
}

void Generate::CheckFiles()
{
	struct dirent *ent;
	DIR *dp;
	char path[MAXPATHLEN];
	dp = opendir(BASEDIR);
	if (dp == NULL) {
		printf("Error: Path doesn't exist\n");
		return;
	}
	while (1) {
		ent = readdir(dp);
		if (!ent) {
			break;
		}
		if (ent->d_type == DT_REG) {
			printf("Regular file: %s\n", ent->d_name);
		}
		if (ent->d_type == DT_DIR) {
			if (strcmp(ent->d_name, "..") != 0 &&
			    strcmp(ent->d_name, ".") != 0) {
				int path_len;
				path_len = snprintf(path, MAXPATHLEN,
						    "%s", ent->d_name);
				printf("%s\n", path);
				if (path_len >= MAXPATHLEN) {
					printf("Error: Path length has gotten too long\n");
				}
			}
		}
	}
	if (closedir(dp)) {
		printf("Couldn't close '%s'\n", BASEDIR);
	}
}

int Generate::CheckMake()
{
	defaultMakefile = MAKEFILE;
	// Get current working directory
	if (getcwd(cwd, MAXPATHLEN) != NULL) {
		printf("Current working directory: %s\n", cwd);
		struct stat buffer;
		int exist = stat(defaultMakefile, &buffer);
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

void Generate::GenBlankConfig()
{
	char fileName[PATH_MAX];
	snprintf(fileName, sizeof(fileName), "%s.ybf", basename(BASEDIR));
	printf("New build file written as: %s\n", fileName);
	newConfig = fopen(fileName, "w+");
}
