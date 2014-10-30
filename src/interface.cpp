// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include "interface.h"

void printHelp()
{
	printf("Usage: yabs [options] <FILE>\n\n"
	       "yabs, a simple build system.\n"
	       "Options:\n"
	       "\t-n, --new\tCreate a new yabs build file\n"
	       "\t-h, --help\tPrint this dialog\n"
	       "\t-p, --parse\tVerbosely parse configuration file\n"
	       "\t-e, --extract\tExtract dist tarball\n");
}

void catchSig(int sig_num)
{
	switch (sig_num) {
	case 2:
		printf("Interrupt signal caught\n");
	}
	exit(sig_num);
}
