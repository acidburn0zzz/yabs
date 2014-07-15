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

class Generate
{
public:
	char cwd[MAXPATHLEN];
	char *currentDir = get_current_dir_name();
	const char *defaultMakefile;
	FILE *Makefile;
	FILE *newConfig;

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
	int WalkDir(const char *DirName, char *Pattern, int Spec);
	int WalkRecur(const char *DirName, regex_t *Expr, int Spec);
};
#endif
