// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _PARSER_H
#define _PARSER_H

#include <yaml.h>
#include <vector>
#include "profile.h"

class Parser
{
private:
	std::vector<Profile *> Profiles;
	int e_num, p_num = 0;
	enum spec {
		error = 0,
		key,
		value,
		doc_start,
		doc_end,
		block_entry,
		block_seq_strt,
		block_seq_end,
		block_map_strt,
		block_map_end,
	} prs;
	spec token_return;
	FILE *conf;
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
	void ParseValues(int verb_flag);
	void VerboseParser(int format);
	void VoidToken();
	void DeleteProfiles();
	void CheckDocStart();
};

#endif
