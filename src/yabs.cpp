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
	int n_opt, p_opt;
	char *n_opt_arg;
	while (1) {
		static struct option long_options[] = {
			{ "new", optional_argument, &n_opt, 'n' },
			{ "help", no_argument, 0, 'h' },
			{ "debug", no_argument, 0, 'd' },
			{ "parse", optional_argument, &p_opt, 'p' },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;
		int c = getopt_long(argc, argv, ":d::p::hn::m", long_options, &option_index);
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
			n_opt_arg = optarg;
			break;
		case 'p':
			if (argv[2] != NULL)
				Prs.OpenConfig(argv[2]);
			break;
		case ':':
			break;
		}
	}
	return 0;
}
