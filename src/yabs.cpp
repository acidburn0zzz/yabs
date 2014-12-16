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
	char *n_opt_arg;
	char *p_arg;
	signal(SIGINT, catchSig);
	while (1) {
		static struct option long_options[] = {
		    {"new", optional_argument, NULL, 'n'},
		    {"help", no_argument, NULL, 'h'},
		    {"debug", no_argument, NULL, 'd'},
		    {"parse", optional_argument, NULL, 'p'},
		    {"extract", optional_argument, NULL, 'e'},
		    {"verbose", optional_argument, NULL, 'v'},
		    {0, 0, 0, 0}};
		int option_index = 0;
		int c = getopt_long(argc, argv, ":d::p:hn::e::v::",
				    long_options, &option_index);
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
			Ybs.WalkDir(Ybs.GetCurrentDir(), ".\\.cpp$",
				    FS_DEFAULT | FS_MATCHDIRS);
			Ybs.PrintFileList();
			if (argv[2] != NULL)
				Ybs.OpenConfig(argv[2], 0);
			break;
		case 'h':
			printHelp();
			break;
		case 'n':
			n_opt_arg = optarg;
			Ybs.GenConfig(0);
			if (n_opt_arg != NULL)
				Ybs.GenConfig(1);
			break;
		case 'p':
			p_arg = optarg;
			if (p_arg != NULL)
				Ybs.OpenConfig(argv[2], 0);
			break;
		case 'v':
			if (argv[2] != NULL)
				Ybs.OpenConfig(argv[2], 1);
		case ':':
			break;
		}
	}
	return EXIT_SUCCESS;
}
