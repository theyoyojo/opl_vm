#include "lex.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char buf[BUFSZ] ;

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

inline int ismetachar(char c) {
	return c == '(' ||
		c == '[' ||
		c == ')' ||
		c == ']' ||
		c == ';' ;
}

int isprim(char * token) {
	for(size_t i = 0; i < PRIM_TABSZ; ++i) {
		if (!strcmp(token, prim_tab[i])) {
			return 1 ;
		}
	}
	return 0 ;
}
/* returns 1 if it got the last token, else returns 0 */

tok_t buf_to_enum(lex_flags_t flags) ;
inline tok_t buf_to_enum(lex_flags_t flags) {
	/* check if tok is number */
	if (flags & LEX_COMMENT) {
		if (!strncmp("%%", buf, BUFSZ)) {
			return T_COMMENT ;
		}
	}
	if (flags & LEX_DIRECTIVE
			&& buflen > 0
			&& buf[0] == '@') {
		return T_DIRECTIVE ;
	}
	else if ((value = atof(buf))) {
		return T_NUM ;
	}
	/* special case: the number 0 */
	else if (*buf == '0' && *(buf+1) == '\0') {
		return T_NUM ;
	}
	else if (flags & LEX_T_STR
			&& buflen > 1 
			&& buf[0] == '"'
			&& buf[buflen - 1] == '"' ) {
		buf[buflen - 1] = '\0' ; /* remove last quote so parse1 can escape it */
		return T_STR;
	}
	/* handle metachars */
	else if (ismetachar(*buf)) {
		switch(*buf) {
			case '(':
			case '[':
				return T_LPAR ;
			case ')':
			case ']':
				return T_RPAR ;
			case ';':
				return T_NULL ;
		}
	}
	else if (isprim(buf)) {
		return T_PRIM ;
	}
	
	/* no matching token? must be an id */
	return T_ID ;
}

/* the lexer */
tok_t get_next_token(FILE * infile, lex_flags_t flags) {
	memset(buf, 0, BUFSZ) ;
	buflen = 0 ;
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
				buflen++ ;
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
			buflen++ ;
		}
	}
	/* once we see the first token, we know that there will be more than zero tokens */
	no_tokens = 0 ;
	*bufp++ = '\0' ;

	return buf_to_enum(flags) ;
}
