#include <stdio.h>
#include "interface.h"

void printHelp()
{
	printf("Usage: yabs [options]\n\n"
	       "yabs, a simple build system.\n"
	       "Options:\n"
	       "-new\tCreate a new yabs build file\n"
	       "-help\tPrint this dialog\n");
}
