#ifndef 	DIRECTIVE_H
#define 	DIRECTIVE_H

#include <stdbool.h>
#include <stdio.h>

int directive_start(void) ;

int directive_end(void) ;

int directive_exec(char * buf, size_t buflen, FILE * outfile) ;

#endif /* DIRECTIVE_H */
