#ifndef _PARSER_H
#define _PARSER_H

#include <yaml.h>

class Parser
{
public:
	FILE *conf;
	yaml_parser_t parser;
	yaml_token_t token;
	Parser();
	~Parser();
	int ParseConfig();
	int OpenConfig(const char *build_file);
	int CloseConfig();
	int ReadValues();
};

#endif
