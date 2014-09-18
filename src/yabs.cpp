// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the copyright holder nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIA DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <iostream>
#include <unistd.h>
#include <getopt.h>
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
