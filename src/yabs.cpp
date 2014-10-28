// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include "dist.h"
#include "interface.h"
#include "yabs.h"

int main(int argc, char *argv[])
{
	Yabs Ybs;
	int n_opt, p_opt;
	char *n_opt_arg;
	while (1) {
		static struct option long_options[] = {
			{ "new", optional_argument, &n_opt, 'n' },
			{ "help", no_argument, 0, 'h' },
			{ "debug", no_argument, 0, 'd' },
			{ "parse", optional_argument, &p_opt, 'p' },
			{ "extract", optional_argument, 0, 'e' },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;
		int c = getopt_long(argc, argv, ":d::p::hn::e::", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'e':
			if (argv[2] != NULL)
				extract(argv[2]);
			break;
		case 'd':
			Ybs.CheckMake();
			Ybs.WriteMake();
			Ybs.GenMakeFromTemplate();
			Ybs.WalkDir(Ybs.GetCurrentDir(), ".\\.cpp$", FS_DEFAULT | FS_MATCHDIRS);
			Ybs.WalkDir(Ybs.GetCurrentDir(), ".\\.h$", FS_DEFAULT | FS_MATCHDIRS);
			Ybs.PrintFileList();
			if (argv[2] != NULL)
				Ybs.OpenConfig(argv[2]);
			break;
		case 'h':
			printHelp();
			break;
		case 'n':
			Ybs.GenBlankConfig(0);
			n_opt_arg = optarg;
			break;
		case 'p':
			if (argv[2] != NULL)
				Ybs.OpenConfig(argv[2]);
			break;
		case ':':
			break;
		}
	}
	return 0;
}
