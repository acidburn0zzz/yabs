#ifndef _GEN_H
#define _GEN_H

#define MAKEFILE "Makefile"
#define FS_NONE 0
#define FS_RECURSIVE (1 << 0)
#define FS_DEFAULT FS_RECURSIVE
#define FS_FOLLOWLINK (1 << 1)
#define FS_DOTFILES (1 << 2)
#define FS_MATCHDIRS (1 << 3)

#ifdef __WIN32__
#include <windows.h>
#endif

#include <regex.h>
#include <unistd.h>
#include <sys/param.h>
#include <fstream>

class Generate
{
public:
	char cwd[MAXPATHLEN];
#ifdef __linux__
	char *current_dir = get_current_dir_name();
#endif
#ifdef __WIN32__
	char *current_dir;
#endif
	const char *default_makefile;
	FILE *makefile;
	FILE *new_config;

	Generate();
	~Generate();
	char *DefineBaseDir();
	void WriteMake();
	void Walk();
	void GenBlankConfig();
	void CheckFiles();
	int CheckConfigExists();
	int CheckMake();
	int GenMakeFromTemplate();
	int WalkDir(const char *dir_name, char *pattern, int spec);
	int WalkRecur(const char *dir_name, regex_t *expr, int spec);
};
#endif
