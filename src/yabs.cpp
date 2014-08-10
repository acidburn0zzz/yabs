#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include "gen.h"
#include "interface.h"
#include "parser.h"

int main(int argc, char *argv[])
{
	Generate Gen;
	Parser Prs;
	char *ybf;
	while (1) {
		static struct option long_options[] = {
			{ "new", optional_argument, 0, 'n' },
			{ "help", no_argument, 0, 'h' },
			{ "debug", no_argument, 0, 'd' },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;
		int c = getopt_long(argc, argv, "dhnm", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'd':
			Gen.CheckMake();
			Gen.WriteMake();
			Gen.GenMakeFromTemplate();
			Gen.WalkDir(Gen.current_dir, ".\\.cpp$", FS_DEFAULT | FS_MATCHDIRS);
			Gen.WalkDir(Gen.current_dir, ".\\.h$", FS_DEFAULT | FS_MATCHDIRS);
			if (argv[2] != NULL)
				Prs.OpenConfig(argv[2]);
			break;
		case 'h':
			printHelp();
			break;
		case 'n':
			Gen.GenBlankConfig(0);
			break;
		}
	}
	return 0;
}
