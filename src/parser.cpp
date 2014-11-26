// Copyright (c) 2013-2014, Alberto Corona <alberto@0x1a.us>
// All rights reserved. This file is part of yabs, distributed under the BSD
// 3-Clause license. For full terms please see the LICENSE file.

#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <yaml.h>
#include "parser.h"
#include "gen.h"

Parser::Parser(){};
Parser::~Parser(){};

int Parser::OpenConfig(const char *build_file, int verb_flag)
{
	if (AssertYML(build_file) == 1) {
		conf = fopen(build_file, "r");
		if (conf == NULL) {
			printf("Error: No file named %s\n", build_file);
			return -2;
		}
		ParseConfig();
		ParseValues(verb_flag);
		CloseConfig();
		return 1;
	} else {
		return -1;
	}
	return 0;
}

int Parser::ParseConfig()
{
	if (conf != NULL) {
		yaml_parser_initialize(&parser);
		yaml_parser_set_input_file(&parser, conf);
		return 1;
	}
	return 0;
}

int Parser::CloseConfig()
{
	if ((&parser != NULL) && (conf != NULL)) {
		yaml_parser_delete(&parser);
		fclose(conf);
		return 1;
	} else {
		return -1;
	}
	return 0;
}

int Parser::AssertYML(const char *build_file)
{
	const char *ext;
	ext = strrchr(build_file, '.');
	if (opendir(build_file) != NULL) {
		printf("Error: %s is a directory\n", build_file);
		return -3;
	}
	if (!ext) {
		printf("Error: %s has no extension\n", build_file);
		return -1;
	}
	if ((strcasecmp(ext + 1, "yml") == 0) || (strcasecmp(ext + 1, "yaml") == 0) ||
	    (strcasecmp(ext + 1, "ybf") == 0)) {
		return 1;
	} else {
		printf("Error: %s is not a valid build file\n", build_file);
		return -2;
	}
	return 0;
}

const char *Parser::ParseValues(int verb_flag)
{
	switch (verb_flag) {
		case 0:
			do {
				ReadValues();
			} while (token_return != 0);
			break;
		case 1:
			do {
				VerboseParser(0);
			} while (token_return != 0);
			break;
	}
	return NULL;
}

const char *Parser::ReadValues()
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
			printf("Tag directive: %s\n", token.data.scalar.value);
			break;
		case YAML_DOCUMENT_START_TOKEN:
			IncDocNum();
			break;
		case YAML_DOCUMENT_END_TOKEN:
			printf("...\n");
			break;
		case YAML_BLOCK_SEQUENCE_START_TOKEN:
			printf("\n");
			break;
		case YAML_BLOCK_END_TOKEN:
			break;
		case YAML_BLOCK_MAPPING_START_TOKEN:
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
			printf("Alias token: %s\n", token.data.scalar.value);
			break;
		case YAML_ANCHOR_TOKEN:
			printf("Anchor token: %s\n", token.data.scalar.value);
			break;
		case YAML_TAG_TOKEN:
			printf("Tag token: %s\n", token.data.scalar.value);
			break;
		case YAML_SCALAR_TOKEN:
			switch (prs) {
			case error:
				break;
			case key:
				printf("%s: ", token.data.scalar.value);
				if (CompValid(token.data.scalar.value) == 1) {
					key_value = ConvValue(token.data.scalar.value);
					token_return = key;
					break;
				} else {
					printf("Error: '%s' is not a valid configuration option\n", token.data.scalar.value);
					token_return = error;
					token.type = YAML_STREAM_END_TOKEN;
					break;
				}
				break;
			case block_entry:
				printf(" - %s\n", token.data.scalar.value);
				PopValidValue(key_value, ConvValue(token.data.scalar.value));
				token_return = block_entry;
				break;
			case value:
				printf("%s\n", token.data.scalar.value);
				PopValidValue(key_value, ConvValue(token.data.scalar.value));
				token_return = value;
				break;
			default:
				printf("%s\n", token.data.scalar.value);
				break;
			}
			break;
		}
		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete(&token);
	} while (token.type != YAML_STREAM_END_TOKEN);
	token_return = error;
	return NULL;
}

void Parser::VerboseParser(int format)
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
			printf("Tag directive: %s\n", token.data.scalar.value);
			break;
		case YAML_DOCUMENT_START_TOKEN:
			printf("---\n");
			IncDocNum();
			break;
		case YAML_DOCUMENT_END_TOKEN:
			printf("...\n");
			break;
		case YAML_BLOCK_SEQUENCE_START_TOKEN:
			printf("\n[Block Sequence Start]\n");
			break;
		case YAML_BLOCK_END_TOKEN:
			printf("[Block Sequence End]\n");
			break;
		case YAML_BLOCK_MAPPING_START_TOKEN:
			printf("[Block Mapping Start]\n");
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
			printf("Alias token: %s\n", token.data.scalar.value);
			break;
		case YAML_ANCHOR_TOKEN:
			printf("Anchor token: %s\n", token.data.scalar.value);
			break;
		case YAML_TAG_TOKEN:
			printf("Tag token: %s\n", token.data.scalar.value);
			break;
		case YAML_SCALAR_TOKEN:
			switch (prs) {
			case error:
				break;
			case key:
				printf("[Key Token]\t");
				printf("%s: ", token.data.scalar.value);
				if (CompValid(token.data.scalar.value) == 1) {
					key_value = ConvValue(token.data.scalar.value);
					token_return = key;
					break;
				} else {
					printf("Error: '%s' is not a valid configuration option\n", token.data.scalar.value);
					token_return = error;
					token.type = YAML_STREAM_END_TOKEN;
					break;
				}
				break;
			case block_entry:
				printf("[Block Entry]\t\t");
				printf("- %s\n", token.data.scalar.value);
				token_return = block_entry;
				break;
			case value:
				printf("[Value Token]\t");
				printf("%s\n", token.data.scalar.value);
				token_return = value;
				break;
			default:
				token_return = error;
				break;
			}
			break;
		}
		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete(&token);
	} while (token.type != YAML_STREAM_END_TOKEN);
	token_return = error;
}
