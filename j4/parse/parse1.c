/*
 * J4 raw sexpr -> python compiler component
 * It's a parser
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

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
	}
	fprintf(stderr, "token buffer: %s, 1stchar=[%d]\n", buf, *buf) ;
}
#define MAX_DEPTH 1024

int main(int argc, char * argv[]) {
	(void)argc, (void)argv ;

	infile = stdin ;
	outfile = stdout ;
	islasttok = 0 ;

	int list_depth = 0 ;
	int list_elcnt[MAX_DEPTH] ;
	memset(list_elcnt, 0, MAX_DEPTH * sizeof(int)) ;

	tok_t next_token ;


/* the parser */
scan:
	next_token = get_next_token(infile, LEX_DEFAULT | LEX_T_STR) ;
	if(err) {
		pr_err() ;
		goto done ;
	}

	switch(next_token) {
		case T_NUM: /* yeah these are the same, I made a mistake keeping them seperate */
		case T_ID:
		case T_PRIM:
		case T_STR:
			if (list_elcnt[list_depth]++ > 0) {
				fprintf(outfile, " (") ;
			}
			if (next_token == T_STR) {
				fprintf(outfile, "\\%s\\\"", buf) ;
			}
			else {
				fprintf(outfile, "%s", buf) ;
			}
			break ;
		case T_LPAR:
			fprintf(outfile, "(") ;
			if (list_elcnt[list_depth]++ > 0) {
				fprintf(outfile, "(") ;
			}
			list_depth++ ;
			Printf("\nLIST_DEPTH: %d\n", list_depth) ;
			break ;
		case T_RPAR:
			if (list_depth == 0) {
				err = E_PAR_NOMATCH ;
				pr_err() ;
				goto done ;
			}
			fprintf(outfile, " ;") ;
			if (list_elcnt[list_depth] == 0) {
				/* empty list () -> (;;) I guess */
				fprintf(outfile, ";)") ;
			}
			while(list_elcnt[list_depth] > 0) {
				fprintf(outfile, ")") ;
				--list_elcnt[list_depth] ;
			}
			list_elcnt[list_depth--] = 0 ; /* pop elcnt off "stack" */
			Printf("\nLIST_DEPTH: %d\n", list_depth) ;
			break ;
		case T_EOF:
		case T_MYSTERY:
		default:
			break ;

	}
	if (list_depth >= MAX_DEPTH) {
		err = E_TOO_DEEP ;
		pr_err() ;
		goto done ;
	}
	
	/* save last token for next parse */
	/* prev_token = next_token ; */
	if (!islasttok) goto scan ;
done:
	fprintf(outfile, "\n") ;
	if (no_tokens) {
		err = E_NO_INPUT ;
		pr_err() ;
	}
	return retcode ;
}
