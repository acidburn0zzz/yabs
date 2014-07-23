#include "gen.h"

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
#define BASEDIR DefineBaseDir()

Generate::Generate() {};
Generate::~Generate() {};

char *Generate::DefineBaseDir()
{
	return currentDir;
}

enum {
	FS_OK = 0,
	FS_BADPattern,
	FS_NAMETOOLONG,
	FS_BADIO,
};

int Generate::WalkRecur(const char *DirName, regex_t *Expr, int Spec)
{
	struct dirent *Ent;
	DIR *Dir;
	char PathName[FILENAME_MAX];
	int Res = FS_OK;
	int len = strlen(DirName);
	if (len >= FILENAME_MAX - 1)
		return FS_NAMETOOLONG;
	strcpy(PathName, DirName);
	PathName[len++] = '/';
	if (!(Dir = opendir(DirName))) {
		printf("Error: Can't open %s", DirName);
		return FS_BADIO;
	}
	errno = 0;
#ifdef __linux__
	struct stat St;
	while ((Ent = readdir(Dir))) {
		if (!(Spec & FS_DOTFILES) && Ent->d_name[0] == '.')
			continue;
		if (!strcmp(Ent->d_name, ".") || !strcmp(Ent->d_name, ".."))
			continue;
		strncpy(PathName + len, Ent->d_name, FILENAME_MAX - len);
		if (lstat(PathName, &St) == -1) {
			printf("Error: Can't stat %s", PathName);
			Res = FS_BADIO;
			continue;
		}
		if (S_ISLNK(St.st_mode) && !(Spec & FS_FOLLOWLINK))
			continue;
		if (S_ISDIR(St.st_mode)) {
			if ((Spec & FS_RECURSIVE))
				WalkRecur(PathName, Expr, Spec);
			if (!(Spec & FS_MATCHDIRS))
				continue;
		}
		if (!regexec(Expr, PathName, 0, 0, 0))
			printf("%s\n", PathName);
	}
#endif
	if (Dir)
		closedir(Dir);
	return Res ? Res : errno ? FS_BADIO : FS_OK;
}

int Generate::WalkDir(const char *DirName, char *Pattern, int Spec)
{
	regex_t r;
	int Res;
	if (regcomp(&r, Pattern, REG_EXTENDED | REG_NOSUB))
		return FS_BADPattern;
	Res = WalkRecur(DirName, &r, Spec);
	regfree(&r);

	return Res;
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

int Generate::CheckConfigExists()
{
	char fileName[PATH_MAX];
	snprintf(fileName, sizeof(fileName), "%s.ybf", basename(BASEDIR));
	if (access(fileName, F_OK) != -1) {
		return 1;
	} else {
		return -1;
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

void Generate::WriteMake()
{
	if (CheckConfigExists() == 1) {
		printf("yabs build file exists\n");
		return;
	} else {
		printf("yabs build file does not exist\n");
	}
	return;
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
