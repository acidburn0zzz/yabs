// Copyright (c) 2013-2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#define VERS "0.1.2"

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
		    {"make", optional_argument, NULL, 'm'},
		    {"help", no_argument, NULL, 'h'},
		    {"debug", no_argument, NULL, 'd'},
		    {"parse", optional_argument, NULL, 'p'},
		    {"extract", optional_argument, NULL, 'e'},
		    {"verbose", optional_argument, NULL, 'V'},
		    {"version", no_argument, NULL, 'v'},
		    {"build", no_argument, NULL, 'b'},
		    {0, 0, 0, 0}};
		int option_index = 0;
		int c = getopt_long(argc, argv, ":d::p:hn::m:e::V::v:b:",
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
			if (p_arg != NULL) {
				if (Ybs.OpenConfig(argv[2], 0) > 0)
					Ybs.PrintAllProfiles();
			}
			break;
		case 'V':
			if (argv[2] != NULL)
				Ybs.OpenConfig(argv[2], 1);
			break;
		case 'm':
			p_arg = optarg;
			if (p_arg != NULL) {
				if (Ybs.OpenConfig(argv[2], 0) > 0)
					Ybs.WriteProfileMakes();
			}
			break;
		case 'v':
			printf("yabs version %s\n", VERS);
			break;
		case 'b':
			p_arg = optarg;
			if (p_arg != NULL) {
				if (Ybs.OpenConfig(argv[2], 0) > 0)
					Ybs.BuildProfiles();
			}
			break;
		case ':':
			break;
		default:
			break;
		}
	}
	return EXIT_SUCCESS;
}
