#include "gen.h"
#include "interface.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Generate Gen;

	std::string Args;
	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i] != NULL)
			Args = argv[1];
		if (argv[i] == NULL) {
		}
		if ((Args == "-new") || (Args == "--new") || (Args == "-n")) {
			Gen.GenBlankConfig();
			return 1;
		}
		if ((Args == "-help") || (Args == "--help") || (Args == "-h")) {
			printHelp();
		}
		if ((Args == "-debug") || (Args == "--debug")) {
			Gen.CheckMake();
			Gen.CheckFiles();
		}
	}
	return 0;
}
