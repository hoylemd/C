#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* io streams */
extern FILE* source;
extern FILE* out;
extern FILE* error;

/* source line number for listing */
extern int lineno;

#endif