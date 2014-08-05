#include <stdio.h>
#include "interface.h"

void printHelp()
{
	printf("Usage: yabs [options]\n\n"
	       "yabs, a simple build system.\n"
	       "Options:\n"
	       "\t-new\tCreate a new yabs build file\n"
	       "\t-help\tPrint this dialog\n");
}
