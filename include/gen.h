#ifndef _GEN_H
#define _GEN_H
#define MAKEFILE "Makefile"

#include <fstream>
#include <sys/param.h>

class Generate
{
public:
	char cwd[MAXPATHLEN];
	const char *defaultMakefile;
	FILE *Makefile;
	FILE *newConfig;

	Generate();
	~Generate();
	char *DefineBaseDir();
	void WriteMake();
	void GenBlankConfig();
	void CheckFiles();
	void ListDir(const char *Path);
	int CheckMake();
};
#endif
