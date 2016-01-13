// Copyright (c) 2013-2016, Alberto Corona <ac@albertocorona.com>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <yaml.h>
#include "colors.h"
#include "decoder.h"

Decoder::Decoder()
{
	e_num = 0;
	p_num = 0;
	prs = error;
	token_return = error;
}

int Decoder::OpenConfig(const char *build_file, int verb_flag)
{
	if (AssertYML(build_file) == 1) {
		conf = fopen(build_file, "r");
		if (conf == NULL) {
			printf("Error: No file named %s\n", build_file);
			return -2;
		}
		ParseConfig();
		if (ParseValues(verb_flag) < 0) {
			CloseConfig();
			return -1;
		}
		CloseConfig();
		return 1;
	} else {
		return -1;
	}
	return 0;
}

void Decoder::WriteProfileMakes()
{
	if (Profiles.size() == 1) {
		Profiles[0]->WriteMake("Makefile");
		return;
	} else {
		for (int i = 0; i < (int)Profiles.size(); i++) {
			std::string make_name = "Makefile-";
			make_name += Profiles[i]->GetOS();
			Profiles[i]->WriteMake(make_name.c_str());
		}
		return;
	}
}

void Decoder::BuildProfiles()
{
	if (Profiles.size() == 1) {
		Profiles[0]->Build();
		return;
	} else {
		for (unsigned i = 0; i < Profiles.size(); i++) {
			Profiles[i]->Build();
		}
		return;
	}
}

int Decoder::ParseConfig()
{
	if (conf != NULL) {
		yaml_parser_initialize(&parser);
		yaml_parser_set_input_file(&parser, conf);
		return 1;
	}
	return 0;
}

void Decoder::CheckDocStart()
{
	if (prs != doc_start) {
		VoidToken();
		printf("%sError: Must start with document start token%s\n", RED,
		       CRM);
	}
}

int Decoder::CloseConfig()
{
	if (conf != NULL) {
		yaml_parser_delete(&parser);
		fclose(conf);
		return 1;
	} else {
		return -1;
	}
	return 0;
}

int Decoder::AssertYML(const char *build_file)
{
	const char *ext;
	ext = strrchr(build_file, '.');
	if (opendir(build_file) != NULL) {
		printf("%sError: %s is a directory%s\n", RED, build_file, CRM);
		return -3;
	}
	if (!ext) {
		printf("%sError: %s has no extension%s\n", RED, build_file,
		       CRM);
		return -1;
	}
	if ((strcasecmp(ext + 1, "yml") == 0) ||
	    (strcasecmp(ext + 1, "yaml") == 0) ||
	    (strcasecmp(ext + 1, "ybf") == 0)) {
		return 1;
	} else {
		printf("%sError: %s is not a valid build file%s\n", RED,
		       build_file, CRM);
		return -2;
	}
	return 0;
}

int Decoder::ParseValues(int verb_flag)
{
	switch (verb_flag) {
	case 0:
		do {
			if (ReadValues() < 0)
				return -1;
		} while (token.type != YAML_STREAM_END_TOKEN);
		break;
	case 1:
		do {
			VerboseDecoder(0);
		} while (token.type != YAML_STREAM_END_TOKEN);
		break;
	}
	return 0;
}

void Decoder::PrintAllProfiles()
{
	for (int i = 0; i < (int)Profiles.size(); i++) {
		if (i == 0) {
			printf("Profile %d\n", i);
			Profiles[i]->PrintProfile();
		} else {
			printf("\n\nProfile %d\n", i);
			Profiles[i]->PrintProfile();
		}
	}
}

void Decoder::VoidToken()
{
	token_return = error;
	yaml_token_delete(&token);
	token.type = YAML_STREAM_END_TOKEN;
}

int Decoder::ReadValues()
{
	do {
		yaml_parser_scan(&parser, &token);
		switch (token.type) {
		case YAML_VERSION_DIRECTIVE_TOKEN:
			break;
		case YAML_NO_TOKEN:
			break;
		case YAML_STREAM_START_TOKEN:
			break;
		case YAML_STREAM_END_TOKEN:
			break;
		case YAML_KEY_TOKEN:
			prs = key;
			break;
		case YAML_VALUE_TOKEN:
			prs = value;
			break;
		case YAML_TAG_DIRECTIVE_TOKEN:
			printf("Tag directive\n");
			break;
		case YAML_DOCUMENT_START_TOKEN:
			prs = doc_start;
			Profiles.emplace_back(new Profile());
			if (e_num != 0)
				++p_num;
			break;
		case YAML_DOCUMENT_END_TOKEN:
			Profiles[e_num]->CheckBlankValues();
			prs = doc_end;
			++e_num;
			break;
		case YAML_BLOCK_SEQUENCE_START_TOKEN:
			prs = block_seq_strt;
			break;
		case YAML_BLOCK_END_TOKEN:
			prs = block_seq_end;
			break;
		case YAML_BLOCK_MAPPING_START_TOKEN:
			CheckDocStart();
			break;
		case YAML_BLOCK_ENTRY_TOKEN:
			prs = block_entry;
			break;
		case YAML_FLOW_SEQUENCE_START_TOKEN:
			break;
		case YAML_FLOW_SEQUENCE_END_TOKEN:
			break;
		case YAML_FLOW_MAPPING_START_TOKEN:
			break;
		case YAML_FLOW_MAPPING_END_TOKEN:
			break;
		case YAML_FLOW_ENTRY_TOKEN:
			break;
		case YAML_ALIAS_TOKEN:
			printf("Alias token\n");
			break;
		case YAML_ANCHOR_TOKEN:
			printf("Anchor token\n");
			break;
		case YAML_TAG_TOKEN:
			printf("Tag token\n");
			break;
		case YAML_SCALAR_TOKEN:
			switch (prs) {
			case error:
				break;
			case key:
				if (Profiles[p_num]->CompValid(
					token.data.scalar.value) == 1) {
					key_value = Profiles[p_num]->ConvValue(
					    token.data.scalar.value);

					token_return = key;
					break;
				} else {
					printf("%s:\n",
					       token.data.scalar.value);
					printf("%sError: '%s' is not a valid "
					       "configuration option%s\n",
					       RED, token.data.scalar.value,
					       CRM);
					VoidToken();
					return -1;
				}
				break;
			case block_entry:
				if (token_return != block_entry &&
				    token_return != key &&
				    token_return != block_seq_strt &&
				    token_return != block_map_strt) {
					VoidToken();
					return -1;
				}
				Profiles[p_num]->PopValidValue(
				    key_value, Profiles[p_num]->ConvValue(
						   token.data.scalar.value));
				token_return = block_entry;
				break;
			case value:
				Profiles[p_num]->PopValidValue(
				    key_value, Profiles[p_num]->ConvValue(
						   token.data.scalar.value));
				token_return = value;
				break;
			default:
				CheckDocStart();
				VoidToken();
				return -1;
			}
			break;
		}
		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete(&token);

	} while (token.type != YAML_STREAM_END_TOKEN);
	return 0;
}

void Decoder::VerboseDecoder(int format)
{
	do {
		yaml_parser_scan(&parser, &token);
		switch (token.type) {
		case YAML_VERSION_DIRECTIVE_TOKEN:
			break;
		case YAML_NO_TOKEN:
			break;
		case YAML_STREAM_START_TOKEN:
			printf("[Stream Start]\n");
			break;
		case YAML_STREAM_END_TOKEN:
			printf("[Stream End]\n");
			break;
		case YAML_KEY_TOKEN:
			prs = key;
			break;
		case YAML_VALUE_TOKEN:
			prs = value;
			break;
		case YAML_TAG_DIRECTIVE_TOKEN:
			printf("[Tag directive]\n");
			break;
		case YAML_DOCUMENT_START_TOKEN:
			prs = doc_start;
			Profiles.emplace_back(new Profile());
			printf("---\n");
			break;
		case YAML_DOCUMENT_END_TOKEN:
			prs = doc_end;
			++e_num;
			printf("...\n");
			break;
		case YAML_BLOCK_SEQUENCE_START_TOKEN:
			printf("\n[Block Sequence Start]\n");
			prs = block_seq_strt;
			break;
		case YAML_BLOCK_END_TOKEN:
			prs = block_seq_end;
			printf("[Block Sequence End]\n");
			break;
		case YAML_BLOCK_MAPPING_START_TOKEN:
			printf("[Block Mapping Start]\n");
			CheckDocStart();
			prs = block_map_strt;
			break;
		case YAML_BLOCK_ENTRY_TOKEN:
			prs = block_entry;
			break;
		case YAML_FLOW_SEQUENCE_START_TOKEN:
			printf("[Flow Sequence Start]\n");
			break;
		case YAML_FLOW_SEQUENCE_END_TOKEN:
			printf("[Flow Sequence End]\n");
			break;
		case YAML_FLOW_MAPPING_START_TOKEN:
			printf("[Flow Mapping Start]\n");
			break;
		case YAML_FLOW_MAPPING_END_TOKEN:
			printf("[Flow Mapping End]\n");
			break;
		case YAML_FLOW_ENTRY_TOKEN:
			printf("[Flow Entry]\n");
			break;
		case YAML_ALIAS_TOKEN:
			printf("[Alias token]\n");
			break;
		case YAML_ANCHOR_TOKEN:
			printf("[Anchor token]\n");
			break;
		case YAML_TAG_TOKEN:
			printf("[Tag token]\n");
			break;
		case YAML_SCALAR_TOKEN:
			switch (prs) {
			case key:
				printf("[Key Token]\t\t");
				if (Profiles[p_num]->CompValid(
					token.data.scalar.value) == 1) {
					printf("%s: ", token.data.scalar.value);
					token_return = key;
					break;
				} else {
					printf("%s:\n",
					       token.data.scalar.value);
					printf("%sError: '%s' is not a valid "
					       "configuration option%s\n",
					       RED, token.data.scalar.value,
					       CRM);
					VoidToken();
					break;
				}
				break;
			case block_entry:
				if (token_return != block_entry &&
				    token_return != key &&
				    token_return != block_seq_strt &&
				    token_return != block_map_strt) {
					VoidToken();
					break;
				}
				printf("[Block Entry]\t\t\t");
				printf("- %s\n", token.data.scalar.value);
				token_return = block_entry;
				break;
			case value:
				printf("\n[Value Token]\t\t");
				printf("\t%s\n", token.data.scalar.value);
				token_return = value;
				break;
			default:
				CheckDocStart();
				break;
			}
			break;
		}

		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete(&token);

	} while (token.type != YAML_STREAM_END_TOKEN);
}
