#include <iostream>
#include <unistd.h>
#include "gen.h"
#include "interface.h"

int main(int argc, char *argv[])
{
	Generate Gen;
	int flag = 0;
	int c;
	while ((c = getopt(argc, argv, "dhnm")) != -1)
		switch (c) {
		case 'd':
			Gen.CheckMake();
			Gen.WriteMake();
			Gen.GenMakeFromTemplate();
			Gen.WalkDir(Gen.current_dir, ".\\.cpp$", FS_DEFAULT | FS_MATCHDIRS);
			Gen.WalkDir(Gen.current_dir, ".\\.h$", FS_DEFAULT | FS_MATCHDIRS);
			break;
		case 'h':
			printHelp();
			break;
		case 'n':
			Gen.GenBlankConfig();
			break;
		case 'm':
			break;
		}
	return 0;
}
