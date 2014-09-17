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

#ifndef _GEN_H
#define _GEN_H

#define MAKEFILE "Makefile"
#define FS_NONE 0
#define FS_RECURSIVE (1 << 0)
#define FS_DEFAULT FS_RECURSIVE
#define FS_FOLLOWLINK (1 << 1)
#define FS_DOTFILES (1 << 2)
#define FS_MATCHDIRS (1 << 3)

#include <regex.h>
#include <unistd.h>
#include <sys/param.h>
#include <fstream>
#include <string>
#include <vector>

enum { FS_OK = 0,
       FS_BADPattern,
       FS_NAMETOOLONG,
       FS_BADIO,
};

class Generate
{
private:
	char cwd[MAXPATHLEN];
#ifdef __unix__
	char *current_dir = getcwd(cwd, MAXPATHLEN);
#endif
	std::vector<std::string> FileList;
	char *file_name;
	const char *default_makefile;
	int file_count = 0;
	FILE *makefile;
	FILE *new_config;

public:
	Generate();
	~Generate();
	char *GetCurrentDir();
	char *RelPathName(char *to_rel);
	int WriteMake();
	void Walk();
	void GenFileList(char *file_list);
	void GenBlankConfig(int force_opt);
	void CheckFiles();
	void PrintFileList();
	int CheckConfigExists();
	int CheckMake();
	int GenMakeFromTemplate();
	int WalkDir(const char *dir_name, const char *pattern, int spec);
	int WalkRecur(const char *dir_name, regex_t *expr, int spec);
};
#endif
