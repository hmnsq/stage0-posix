/* Copyright (C) 2020 Jeremiah Orians
 * This file is part of stage0.
 *
 * stage0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * stage0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with stage0.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

char** tokens;
ціле command_done;

/* Function for purging line comments */
void collect_comment(FILE* input)
{
	ціле c;
	do
	{
		c = fgetc(input);
		якщо(EOF == c) exit(EXIT_FAILURE);
	} поки('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
ціле collect_string(FILE* input, ціле index, char* target)
{
	ціле c;
collect_string_loop:
	c = fgetc(input);
	якщо(EOF == c) exit(EXIT_FAILURE);
	інакше якщо('"' == c) повернути index;
	target[index] = c;
	index = index + 1;
	goto collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
char* collect_token(FILE* input)
{
	char* token = calloc(max_string, sizeof(char));
	ціле c;
	ціле i = 0;
collect_token_loop:
	c = fgetc(input);
	якщо(EOF == c) exit(EXIT_SUCCESS);
	інакше якщо((' ' == c) || ('\t' == c)) goto collect_token_done;
	інакше якщо('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		goto collect_token_done;
	}
	інакше якщо('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(input, i, token);
		goto collect_token_done;
	}
	інакше якщо('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(input);
		command_done = 1;
		goto collect_token_done;
	}
	інакше якщо('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(input);
		goto collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	goto collect_token_loop;

collect_token_done:
	якщо(0 == i) повернути NULL;
	повернути token;
}

ціле main(ціле argc, char** argv, char** envp)
{
	char* filename = "kaem.run";
	FILE* script = NULL;

	якщо(NULL != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	якщо(NULL == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, sizeof(char*));

	ціле i = 0;
	ціле status = 0;
	command_done = 0;
	do
	{
		char* result = collect_token(script);
		якщо(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} поки(0 == command_done);

	якщо(0 > i) goto main_loop;

	fputs(" +> ", stdout);
	ціле j;
	для(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	char* program = tokens[0];
	якщо(NULL == program) exit(EXIT_FAILURE);

	ціле f = fork();
	якщо (f == -1) exit(EXIT_FAILURE);
	інакше якщо (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	якщо(0 == status) goto main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
