#include <fstream>
#include <yaml.h>
#include "parser.h"

Parser::Parser() {};
Parser::~Parser() {};

int Parser::OpenConfig(const char *build_file)
{
	conf = fopen(build_file, "r");
	ParseConfig();
	ReadValues();
	CloseConfig();
	return 0;
}

int Parser::ParseConfig()
{
	if (conf != NULL) {
		yaml_parser_initialize(&parser);
		yaml_parser_set_input_file(&parser, conf);
	}
	return 0;
}

int Parser::CloseConfig()
{
	if (&parser != NULL)
		yaml_parser_delete(&parser);
	if (conf != NULL) {
		fclose(conf);
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
			printf("No token\n");
			break;
		case YAML_STREAM_START_TOKEN:
			printf("Stream start\n");
			break;
		case YAML_STREAM_END_TOKEN:
			printf("Stream end\n");
			break;
		case YAML_KEY_TOKEN:
			printf("\tKey token: ");
			break;
		case YAML_VALUE_TOKEN:
			printf("\t\tValue token: ");
			break;
		case YAML_TAG_DIRECTIVE_TOKEN:
			printf("Tag directive: %s\n", token.data.scalar.value);
			break;
		case YAML_DOCUMENT_START_TOKEN:
			printf("Document start\n");
			break;
		case YAML_DOCUMENT_END_TOKEN:
			printf("Document end\n");
			break;
		case YAML_BLOCK_SEQUENCE_START_TOKEN:
			printf("Block sequence start\n");
			break;
		case YAML_BLOCK_END_TOKEN:
			printf("Block sequence end\n");
			break;
		case YAML_BLOCK_MAPPING_START_TOKEN:
			printf("Block Mapping start\n");
			break;
		case YAML_FLOW_SEQUENCE_START_TOKEN:
			printf("Sequence start\n");
			break;
		case YAML_FLOW_SEQUENCE_END_TOKEN:
			printf("Sequence end\n");
			break;
		case YAML_FLOW_MAPPING_START_TOKEN:
			printf("Mapping start\n");
			break;
		case YAML_FLOW_MAPPING_END_TOKEN:
			printf("Mapping end\n");
			break;
		case YAML_BLOCK_ENTRY_TOKEN:
			printf("Block entry\n");
			break;
		case YAML_FLOW_ENTRY_TOKEN:
			printf("Block entry token: %s\n", token.data.scalar.value);
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
			printf("%s\n", token.data.scalar.value);
			break;
		}
	} while (token.type != YAML_STREAM_END_TOKEN);
	yaml_token_delete(&token);
	return 0;
}
