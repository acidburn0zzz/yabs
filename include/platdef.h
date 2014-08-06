#ifndef _PLATDEF
#define _PLATDEF

#ifdef __linux__
int lin_def = 1;
int win_def = 0;
#endif

#ifdef __WIN32__
int lin_def = 0;
int win_def = 1;
#endif

#endif
