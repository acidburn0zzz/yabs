// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PARSER_H
#define _PARSER_H

#include <yaml.h>
#include "profile.h"

class Parser : public Profile
{
private:
	enum spec {
		error = 0,
		key,
		value,
		block_entry,
	} prs;
	spec token_return;
	FILE *conf;
	int valid;
	std::string key_value;
	yaml_parser_t parser;
	yaml_token_t token;

public:
	Parser();
	~Parser();
	int AssertYML(const char *build_file);
	int ParseConfig();
	int OpenConfig(const char *build_file, int verb_flag);
	int CloseConfig();
	const char *ReadValues();
	const char *ParseValues(int verb_flag);
	void VerboseParser(int format);
};

#endif
