/*
 * J4 raw sexpr -> python compiler component
 * It's a preprocessor
 *
 * This is the first parse layer in J4
 *
 * comment := "%.*\n" | "%%.*%%"
 *
 * directive := "@.*\n" & ( given | search)
 *
 * search := "@search<dir>"
 *
 * given := "@given<lib>"
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lex.h"
#include "directive.h"

FILE * infile ;
FILE * outfile ;
int no_tokens = 1 ;
int retcode = 0 ;

enum error {
	E_NO_ERR = 0,
	E_MYSTERY_TOK,
	E_SUPRISE_EOF,
	E_TOO_DEEP,
	E_PAR_NOMATCH,
	E_NO_INPUT,
	E_MANY_TOPFS,
	E_DIRECTIVE,
} err = E_NO_ERR ;

void pr_err(void) {
	retcode = 1 ;
	switch (err) {
		case E_NO_ERR:
			fprintf(stderr, "There is no error...\n") ;
			break ;
		case E_MYSTERY_TOK:
			fprintf(stderr, "Error: Unknown token found in input\n") ;
			break ;
		case E_SUPRISE_EOF:
			fprintf(stderr, "Error: Encountered unexpected EOF while parsing\n") ;
			break ;
		case E_TOO_DEEP:
			fprintf(stderr, "Error: Cons depth exceeds max.\n") ;
			break ;
		case E_PAR_NOMATCH:
			fprintf(stderr, "Error: Found ) with no mathing (.\n") ;
			break ;
		case E_NO_INPUT:
			fprintf(stderr, "Error: There is no input.\n") ;
			break ;
		case E_MANY_TOPFS:
			fprintf(stderr, "Error: There can only be one topform in J4\n") ;
			break ;
		case E_DIRECTIVE:
			fprintf(stderr, "Error: Directve Error (in parse context).\n") ;
			break ;
	}
	fprintf(stderr, "token buffer: %s, 1stchar=[%d]\n", buf, *buf) ;
}

#define LINE_WRAP 80

void maybe_wrap(size_t * accum, size_t extra) ;
inline void maybe_wrap(size_t * accum, size_t extra) {

	if (*accum + extra > LINE_WRAP) {
		*accum = 0 ;
		fputc('\n', outfile) ;
	}
}

int main(int argc, char * argv[]) {
	(void)argc, (void)argv ;

	infile = stdin ;
	outfile = stdout ;
	islasttok = 0 ;
	
	bool iscomment = false ;
	size_t bufaccum  = 0 ;
	long appendages = 0 ;
	long directive_ret ;

	tok_t next_token ;

	directive_start() ;

/* the parser */
scan:
	next_token = get_next_token(infile, LEX_DEFAULT | LEX_COMMENT | LEX_DIRECTIVE) ;
	if(err) {
		pr_err() ;
		goto done ;
	}
	maybe_wrap(&bufaccum, buflen) ;
	bufaccum += buflen + 1 ;

	switch(next_token) {
		case T_NUM:
		case T_ID:
		case T_PRIM:
		case T_LPAR:
		case T_RPAR:
			if (iscomment) break ;
			fprintf(outfile, "%s ", buf) ;
			break ;
		case T_COMMENT:
			iscomment = !iscomment ;
			break ;
		case T_DIRECTIVE:
			directive_ret = directive_exec(buf, buflen, outfile) ;
			if (directive_ret < 0) {
				err = E_DIRECTIVE ;
				pr_err() ;
			}
			else {
				appendages += directive_ret ;
			}
			break ;
		case T_EOF:
		case T_MYSTERY:
		default:
			break ;

	}
	/* save last token for next parse */
	/* prev_token = next_token ; */
	if (!islasttok) goto scan ;
done:
	fprintf(outfile, "\n") ;
	for (int i = 0; i < appendages; ++i) {
		fputc(')', outfile) ;
	}
	fprintf(outfile, "\n") ;
	if (no_tokens) {
		err = E_NO_INPUT ;
		pr_err() ;
	}
	directive_end() ;
	return retcode ;
}
