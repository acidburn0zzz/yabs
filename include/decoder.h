// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#ifndef _Decoder_H
#define _Decoder_H

#include <yaml.h>
#include <vector>
#include <memory>
#include "profile.h"

class Decoder
{
private:
	std::vector<std::unique_ptr<Profile>> Profiles;
	int e_num, p_num;
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
	Decoder();
	int AssertYML(const char *build_file);
	int ParseConfig();
	int OpenConfig(const char *build_file, int verb_flag);
	int CloseConfig();
	int ReadValues();
	int ParseValues(int verb_flag);
	void VerboseDecoder(int format);
	void VoidToken();
	void CheckDocStart();
	void PrintAllProfiles();
	void WriteProfileMakes();
	void BuildProfiles();
};

#endif
