#ifndef  	LEX_H
#define 	LEX_H

#include <stdio.h>

typedef unsigned long lex_flags_t ;

#define 	LEX_DEFAULT 	0x0UL
#define 	LEX_DIRECTIVE 	0x1UL /* directives are metacharacters */
#define 	LEX_COMMENT 	0x2UL 
#define 	LEX_T_STR 	0x4UL 

#define BUFSZ 256
extern char buf[] ;
size_t buflen ;
double value ;

extern int retcode ;

typedef enum token {
	T_NUM,
	T_PRIM,
	T_ID,
	T_NULL,
	T_RPAR,
	T_LPAR,
	T_EOF,
	T_STR,
	T_COMMENT,
	T_DIRECTIVE,
	T_MYSTERY = 99,
} tok_t ;

int islasttok ;
extern int no_tokens ;

extern char * prim_tab[] ;

int ismetachar(char c) ;
tok_t get_next_token(FILE * infile, lex_flags_t flags) ;

#define PRIM_TABSZ sizeof(prim_tab)/8


#ifdef DEBUG
#define Printf(...) printf(__VA_ARGS__)
#else
#define Printf(...) (void)0
#endif

#endif /* LEX_H */
