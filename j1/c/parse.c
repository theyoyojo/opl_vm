/*
 * J1 raw sexpr -> python compiler component
 * It's a lexer and a parser
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE * infile ;
FILE * outfile ;
int retcode = 0 ;
int no_tokens = 1 ;

#define BUFSZ 256
char buf[BUFSZ] ;
size_t bufflen ;
double value ;

typedef enum token {
	T_NUM,
	T_PRIM,
	T_NULL,
	T_RPAR,
	T_LPAR,
	T_EOF,
	T_MYSTERY = 99,
} tok_t ;

int islasttok ;

char * prim_tab[] = {
	"+",
	"-",
	"/",
	"*",
	">",
	">=",
	"=",
	"<",
	"<=",
	"if",
} ;

#define PRIM_TABSZ sizeof(prim_tab)/8

int isprim(char * token) {
	for(size_t i = 0; i < PRIM_TABSZ; ++i) {
		if (!strcmp(token, prim_tab[i])) {
			return 1 ;
		}
	}
	return 0 ;
}

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
			fprintf(stderr, "Error: There can only be one topform in J1\n") ;
			break ;
	}
	fprintf(stderr, "token buffer: %s, 1stchar=[%d]\n", buf, *buf) ;
}

int ismetachar(char c) ;
inline int ismetachar(char c) {
	return c == '(' ||
		c == ')' ||
		c == ';' ;
}

/* returns 1 if it got the last token, else returns 0 */
/* the lexer */
tok_t get_next_token(FILE * infile) {
	memset(buf, 0, BUFSZ) ;
	bufflen = 0 ;
	retcode = 0 ;
	char c ;
	char * bufp = buf ;
	/* ingnore leading whitespace */
	while(isspace(c = fgetc(infile))) ;
	/* when we get a non-whitespace character, put it back in the stream */
	/* if we have leading whitespace and then end of file, we are done */
	if (c == EOF) {
		islasttok = 1 ;
		return T_EOF ;
	}
	/* if we find a proper character, but it back in the stream and lex some more */
	ungetc(c, infile) ;

	while (!isspace(c = fgetc(infile))) {
		if (c == EOF) {
			islasttok = 1 ;
		}
		/* metachars are single characters, if we see one we are done */
		if (ismetachar(c)) { 
			/* if this is the first character, the metacharacter is the token */
			if (bufp == buf) {
				*bufp++ = c ;	
				bufflen++ ;
				c = fgetc(infile) ;
				/* printf("next: %d, EOF %d\n", c, EOF) ; */
				ungetc(c, infile) ;
				break ;
			}
			/* but if it is not the first character, save it for next lex */
			else {
				ungetc(c, infile) ;
				break ;
			}
		}
		else {
			/* if it's not a metacharacter, keep going */
			*bufp++ = c ;
			bufflen++ ;
		}
	}
	/* once we see the first token, we know that there will be more than zero tokens */
	no_tokens = 0 ;
	*bufp++ = '\0' ;

	/* check if tok is number */
	if ((value = atof(buf))) {
		return T_NUM ;
	}
	/* special case: the number 0 */
	else if (*buf == '0' && *(buf+1) == '\0') {
		return T_NUM ;
	}
	/* handle metachars */
	else if (ismetachar(*buf)) {
		switch(*buf) {
			case '(':
				return T_LPAR ;
			case ')':
				return T_RPAR ;
			case ';':
				return T_NULL ;
		}
	}
	else if (isprim(buf)) {
		return T_PRIM ;
	}
	
	/* no matching token? err time :) */
	err = E_MYSTERY_TOK ;
	return T_MYSTERY ;
}

#define MAX_DEPTH 1024

int main(int argc, char * argv[]) {
	(void)argc, (void)argv ;

	infile = stdin ;
	outfile = stdout ;
	islasttok = 0 ;

	int cons_depth = 0 ;
	int cons_elcnt[MAX_DEPTH] ;
	int topform_count = 0 ;
	memset(cons_elcnt, 0, MAX_DEPTH * sizeof(int)) ;

	tok_t next_token ;
	tok_t prev_token ;
	fprintf(outfile, "main = ") ;

/* the parser */
scan:
	next_token = get_next_token(infile) ;
	if(err) {
		pr_err() ;
		goto done ;
	}

	switch(next_token) {
		case T_NUM: /* yeah these are the same, I made a mistake keeping them seperate */
		case T_PRIM:
			if (cons_depth == 0) {
				err = E_LONELY_TOK ;
				pr_err() ;
				goto done ;
			}
			if (prev_token == T_RPAR) {
				putchar(','), putchar(' ') ;
			}
			fprintf(outfile, "Atom(\"%s\")", buf) ;
			goto insert_comma_maybe ;
		case T_NULL:
			if (cons_depth == 0) {
				err = E_LONELY_TOK ;
				pr_err() ;
				goto done ;
			}
			if (prev_token == T_RPAR) {
				putchar(','), putchar(' ') ;
			}
			fprintf(outfile, "Nil()") ;
			goto insert_comma_maybe ;
		insert_comma_maybe:
			cons_elcnt[cons_depth]++ ;
			if (cons_elcnt[cons_depth] == 1) {
				putchar(','), putchar(' ') ;
			}
			break ;
		case T_LPAR:
			if (prev_token == T_RPAR) {
				putchar(','), putchar(' ') ;
			}
			/* If we already have a complete topform, we can't have another */
			if (topform_count > 0) {
				err = E_MANY_TOPFS ;
				pr_err() ;
				goto done ;
			}
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
	if (cons_depth > MAX_DEPTH) {
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
