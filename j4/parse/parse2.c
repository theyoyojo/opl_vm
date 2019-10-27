/*
 * J4 raw sexpr -> python compiler component
 * It's a lexer and a parser
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
	E_THICC_CONS,
	E_THIN_CONS,
	E_LONELY_TOK,
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
			fprintf(stderr, "Error: Found ) with no mathing ).\n") ;
			break ;
		case E_THICC_CONS:
			fprintf(stderr, "Error: Cons cannot have more than two elements.\n") ;
			break ;
		case E_THIN_CONS:
			fprintf(stderr, "Error: Cons cannot have less than two elements.\n") ;
			break ;
		case E_LONELY_TOK:
			fprintf(stderr, "Error: Non-cons element outside of top-level cons.\n") ;
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

	size_t bufaccum  = 7 ; /* for main */
	int cons_depth = 0 ;
	int cons_elcnt[MAX_DEPTH] ;
	int topform_count = 0 ;
	memset(cons_elcnt, 0, MAX_DEPTH * sizeof(int)) ;

	tok_t next_token ;
	tok_t prev_token ;
	fprintf(outfile, "main = ") ;

/* the parser */
scan:
	next_token = get_next_token(infile, LEX_DEFAULT) ;
	if(err) {
		pr_err() ;
		goto done ;
	}
	switch(next_token) {
		case T_NUM: /* yeah these are the same, I made a mistake keeping them seperate */
		case T_ID:
		case T_PRIM:
			if (cons_depth == 0) {
				err = E_LONELY_TOK ;
				pr_err() ;
				goto done ;
			}
			if (prev_token == T_RPAR) {
				fputc(',', outfile), fputc(' ', outfile) ;
				bufaccum +=2 ;
			}
			maybe_wrap(&bufaccum, buflen + 8) ;
			bufaccum += buflen + 8 ;
			fprintf(outfile, "Atom(\"%s\")", buf) ;
			goto insert_comma_maybe ;
		case T_NULL:
			if (cons_depth == 0) {
				err = E_LONELY_TOK ;
				pr_err() ;
				goto done ;
			}
			if (prev_token == T_RPAR) {
				fputc(',', outfile), fputc(' ', outfile) ;
				bufaccum +=2 ;
			}
			maybe_wrap(&bufaccum, 5) ;
			bufaccum += 5 ;
			fprintf(outfile, "Nil()") ;
			goto insert_comma_maybe ;
		insert_comma_maybe:
			cons_elcnt[cons_depth]++ ;
			if (cons_elcnt[cons_depth] == 1) {
				fputc(',', outfile), fputc(' ', outfile) ;
				bufaccum +=2 ;
			}
			break ;
		case T_LPAR:
			if (prev_token == T_RPAR) {
				fputc(',', outfile), fputc(' ', outfile) ;
				bufaccum +=2 ;
			}
			/* If we already have a complete topform, we can't have another */
			if (topform_count > 0) {
				err = E_MANY_TOPFS ;
				pr_err() ;
				goto done ;
			}
			maybe_wrap(&bufaccum, 5) ;
			bufaccum += 5 ;
			fprintf(outfile, "Cons(") ;
			cons_depth++ ;
			break ;
		case T_RPAR:
			if (cons_depth == 0) {
				err = E_PAR_NOMATCH ;
				pr_err() ;
				goto done ;
			}
			if (cons_elcnt[cons_depth] < 2) {
				err = E_THIN_CONS;
				pr_err() ;
				goto done ;
			}
			if (cons_elcnt[cons_depth] > 2) {
				err = E_THICC_CONS;
				pr_err() ;
				goto done ;
			}
			maybe_wrap(&bufaccum, 1) ;
			bufaccum += 1 ;
			fprintf(outfile, ")") ;
			cons_elcnt[cons_depth--] = 0 ; /* pop elcnt off "stack" */
			/* if we reach depth 0, we have parsed an entire topfom */
			if (cons_depth == 0) {
				++topform_count ;
			}
			cons_elcnt[cons_depth]++ ;
			break ;
		case T_EOF:
		case T_MYSTERY:
		default:
			break ;

	}
	if (cons_depth >= MAX_DEPTH) {
		err = E_TOO_DEEP ;
		pr_err() ;
		goto done ;
	}
	
	/* save last token for next parse */
	prev_token = next_token ;
	if (!islasttok) goto scan ;
done:
	fputc('\n', outfile) ;
	if (no_tokens) {
		err = E_NO_INPUT ;
		pr_err() ;
	}
	return retcode ;
}
