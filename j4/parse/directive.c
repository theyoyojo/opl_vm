#include "directive.h"
#include <string.h>
#include <stdlib.h>

static char ** libpath ;
static size_t  libpath_capacity ;
static size_t  libpath_length ;

#define PATH_LIMIT 255
int directive_start(void) {
	libpath_capacity = 7 ; /* arbitrary > 1 */
	if (!(libpath = (char **)malloc(sizeof(char *) * libpath_capacity))) {
		fprintf(stderr, "Directive Error: allocation error\n") ;
		return -1 ;
	}

	char * j4_path ;
	if (!(j4_path = getenv("J4_PATH"))) {
		fprintf(stderr, "Directive Error: J4_PATH environment variable unset\n") ;
		return -1 ;
	}
	if (!(libpath[0] = (char *)malloc(sizeof(char) * PATH_LIMIT))) {
		fprintf(stderr, "Directive Error: allocation error\n") ;
		return -1 ;
	}
	libpath_length = 1 ;
	strncpy(libpath[0], j4_path, PATH_LIMIT) ;
	strcat(libpath[0], "/lib") ;

	return 0 ;
}

int directive_end(void) {
	for (size_t i = 0; i < libpath_length; ++i) {
		free(libpath[i]) ;
	}
	free(libpath) ;
	libpath = NULL ;
	return 0 ;
}

int libpath_append(char * path, size_t pathlen) {
	/* case: need to expand libpath capacity */
	if (libpath_length >= libpath_capacity) {
		if (!(libpath = (char **)realloc(libpath, libpath_capacity *= 2))) {
			fprintf(stderr, "Directive Error: allocation error\n") ;
			return -1 ;

		}
	}
	if (!(libpath[libpath_length] = (char *)malloc(sizeof(char) * (pathlen + 1)))) {
		fprintf(stderr, "Directive Error: allocation error\n") ;
		return -1 ;

	}
	strncpy(libpath[libpath_length], path, pathlen) ;
	libpath[libpath_length][pathlen] = '\0' ;

	++libpath_length ;

	return 0 ;
}

void print_libpath(FILE * outfile) {
	fprintf(outfile, "libpath[") ;
	for (size_t i = 0; i < libpath_length; ++i) {
		fprintf(outfile, "%s%s", libpath[i],
				i == libpath_length - 1 ? "" : ", ") ;
	}
	fprintf(outfile, "]\n") ;
}

bool get_command(size_t buflen, char ** command, size_t * command_length) {
	char * cursor = * command ;
	while (*command_length + 1 < buflen && *cursor++ != '<') {
		++*command_length ;
	}
	if (*command_length == 0) {
		fprintf(stderr, "Directive Error: no command\n") ;
		return false ;
	}
	else {
		return true ;
	}
}

int search_exec(int argc, char * argv[], FILE * outfile) {
	/* fprintf(outfile, "SEARCH(%s)\n", argv[0]) ; */
	if (libpath_append(argv[0], strlen(argv[0])) != 0) {
		fprintf(stderr, "Directive Error: cannot append to libpath\n") ;
		return -1 ;
	}
	/* print_libpath(outfile) ; */
	return 0 ;
}

#define GIVEN_BUFSZ	128
int given_exec (int argc, char * argv[], FILE * outfile) {
	static char given_name[GIVEN_BUFSZ] ;
	memset(given_name, 0, GIVEN_BUFSZ) ;

	FILE * given_file ; 

	/* fprintf(outfile, "GIVEN(%s)\n", argv[0]) ; */
	/* print_libpath(outfile) ; */

	strcpy(given_name, libpath[0]) ;
	strcat(given_name, "/") ;
	strcat(given_name, argv[0]) ;
	strcat(given_name, ".given") ;
	/* fprintf(outfile, "%s\n", given_name) ; */

	/* TODO search whole libpath */
	if (!(given_file = fopen(given_name, "r+"))) {
		return -1 ;
	}

	fprintf(outfile, "(let*(\n") ;
	char c ;
	while ((c = fgetc(given_file)) != EOF) {
		fputc(c, outfile) ;
	}
	fputc(')', outfile) ;

	fclose(given_file) ;

	return 1 ;
}

typedef enum _command {
	COMMAND_SEARCH,
	COMMAND_GIVEN,
} command_t ;
	
int command_args[] = {
	[COMMAND_SEARCH] 	= 1,
	[COMMAND_GIVEN] 	= 1,
} ;

int (*command_exec[])(int argc, char * argv[], FILE * outfile) = {
	[COMMAND_SEARCH] 	= search_exec,
	[COMMAND_GIVEN] 	= given_exec,
} ;

#define COMMAND_MAXLEN 	64
/* returns number of args in error case */
int get_args(command_t command_enum, char * start, size_t remaining,
		int * argc, char *** argv) {
	
	if (!(*argv = (char **)malloc(sizeof(char *) * command_args[command_enum]))) {
		fprintf(stderr, "Directive Error: allocation error\n") ;
		return -1 ;
	}
	for (int i = 0; i < command_args[command_enum]; ++i) {
		if (!((*argv)[i] = 
				(char *)malloc(sizeof(char)
					* COMMAND_MAXLEN))) {
			fprintf(stderr, "Directive Error: allocation error\n") ;
			return -1 ;
		}
	}

	size_t current_length = 0, offset = 0 ;
	char * current = start ;
	*argc = 0 ;

	while (offset < remaining) {
		if (current_length >= COMMAND_MAXLEN) {
			fprintf(stderr, "Directive Error: arg too long\n") ;
			return -1 ;

		}
		switch(*current)  {
		case '>':
			/* if this is not the first character, we just finished an arg */
			if (current_length > 0) {
				(*argv)[*argc][current_length] = '\0' ;
				(*argc)++ ;
			}
			goto out;
		case ',':
			(*argv)[*argc][current_length] = '\0';
			current_length = 0 ;
			(*argc)++ ;
			break ;
		default:
			(*argv)[*argc][current_length++] = *current ;
			break ;
		}
		
		offset++ ;
		current++ ;
	}

out:
	return *argc - command_args[command_enum] ;
}

void free_args(int command_argc, char *** command_argv) {
	for (int i = 0; i < command_argc; ++i) {
		free((*command_argv)[i]) ;
	}
	free(*command_argv) ;
	*command_argv = NULL ;
}

int directive_exec(char * buf, size_t buflen, FILE * outfile) {

	command_t command_enum;

	size_t command_length = 0 ;
	char * command = buf + 1 ;

	if (!get_command(buflen, &command, &command_length)) {
		return false ;
	}
	
	if (!strncmp("search", command, command_length)) {
		command_enum = COMMAND_SEARCH ;
	}
	else if (!strncmp("given", command, command_length)) {
		command_enum = COMMAND_GIVEN ;
	}
	else {
		/* this affects the global buffer, but we are done with it
		 * by now in this case
		 */
		command[command_length] = '\0' ;
		fprintf(stderr, "Directive Error: unknown command '%s'\n",
				command) ;
		return false ;
		
	}

	int args_ret, command_argc ;
	char ** command_argv ;
	if ((args_ret = get_args(command_enum, buf + command_length + 2,
					buflen - command_length - 2,
					&command_argc, &command_argv))) {
		fprintf(stderr, "Directive Error: directive '%s' expects %d args, got %d\n",
				command, command_args[command_enum], command_argc) ;
		return false ;

	}

	int directive_ret = command_exec[command_enum](command_argc, command_argv, outfile) ;

	free_args(command_argc, &command_argv) ;
	return directive_ret ;
}
