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

#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <yaml.h>
#include "parser.h"

Parser::Parser(){};
Parser::~Parser(){};

int Parser::OpenConfig(const char *build_file)
{
	if (AssertYML(build_file) == 1) {
		conf = fopen(build_file, "r");
		if (conf == NULL) {
			printf("Error: No file named %s\n", build_file);
			return -2;
		}
		ParseConfig();
		ReadValues();
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
		yaml_token_delete(&token);
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
	if ((strcmp(ext + 1, "yml") == 0) || (strcmp(ext + 1, "yaml") == 0) ||
	    (strcmp(ext + 1, "ybf") == 0)) {
		return 1;
	} else {
		printf("Error: %s is not a valid build file\n", build_file);
		return -2;
	}
	return 0;
}

int Parser::ReadValues()
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
			printf("---\n");
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
				printf("Error: There was an error parsing the file\n");
				return -1;
			case key:
				printf("%s: ", token.data.scalar.value);
				break;
			case block_entry:
				printf(" - %s\n", token.data.scalar.value);
				break;
			case value:
				printf("%s\n", token.data.scalar.value);
				break;
			default:
				printf("%s\n", token.data.scalar.value);
				break;
			}
			break;
		}
	} while (token.type != YAML_STREAM_END_TOKEN);
	return 0;
}
