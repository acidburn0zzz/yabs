#include <stdio.h>
#include "interface.h"

void printHelp()
{
	printf("Usage: yabs [options]\n\n"
	       "yabs, a simple build system.\n"
	       "Options:\n"
	       "\t-n\tCreate a new yabs build file\n"
	       "\t-h\tPrint this dialog\n");
}
