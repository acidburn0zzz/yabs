#include <iostream>
#include "gen.h"
#include "interface.h"

int main(int argc, char *argv[])
{
	Generate Gen;

	std::string Args;
	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i] != NULL)
			Args = argv[1];
		if (argv[i] == NULL) {
		}
		if ((Args == "-new") || (Args == "--new") || (Args == "-n")) {
			Gen.GenBlankConfig();
			return 1;
		}
		if ((Args == "-help") || (Args == "--help") || (Args == "-h")) {
			printHelp();
		}
		if ((Args == "-debug") || (Args == "--debug")) {
			Gen.CheckMake();
			Gen.WriteMake();
			Gen.GenMakeFromTemplate();
			Gen.WalkDir(Gen.currentDir, ".\\.cpp$", FS_DEFAULT | FS_MATCHDIRS);
			Gen.WalkDir(Gen.currentDir, ".\\.h$", FS_DEFAULT | FS_MATCHDIRS);
		}
	}
	return 0;
}
