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

var_na** tokens;
ကိန်းပြည့် command_done;

/* Function for purging line comments */
void collect_comment(FILE* input)
{
	ကိန်းပြည့် c;
	do
	{
		c = fgetc(input);
		အကယ်၍(EOF == c) exit(EXIT_FAILURE);
	} နေစဉ်('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
ကိန်းပြည့် collect_string(FILE* input, ကိန်းပြည့် index, var_na* target)
{
	ကိန်းပြည့် c;
collect_string_loop:
	c = fgetc(input);
	အကယ်၍(EOF == c) exit(EXIT_FAILURE);
	မဟုတ်ပါက အကယ်၍('"' == c) ပြန်ပေး index;
	target[index] = c;
	index = index + 1;
	goto collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
var_na* collect_token(FILE* input)
{
	var_na* token = calloc(max_string, parimaa_na(var_na));
	ကိန်းပြည့် c;
	ကိန်းပြည့် i = 0;
collect_token_loop:
	c = fgetc(input);
	အကယ်၍(EOF == c) exit(EXIT_SUCCESS);
	မဟုတ်ပါက အကယ်၍((' ' == c) || ('\t' == c)) goto collect_token_done;
	မဟုတ်ပါက အကယ်၍('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		goto collect_token_done;
	}
	မဟုတ်ပါက အကယ်၍('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(input, i, token);
		goto collect_token_done;
	}
	မဟုတ်ပါက အကယ်၍('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(input);
		command_done = 1;
		goto collect_token_done;
	}
	မဟုတ်ပါက အကယ်၍('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(input);
		goto collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	goto collect_token_loop;

collect_token_done:
	အကယ်၍(0 == i) ပြန်ပေး NULL;
	ပြန်ပေး token;
}

ကိန်းပြည့် main(ကိန်းပြည့် argc, var_na** argv, var_na** envp)
{
	var_na* filename = "kaem.run";
	FILE* script = NULL;

	အကယ်၍(NULL != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	အကယ်၍(NULL == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, parimaa_na(var_na*));

	ကိန်းပြည့် i = 0;
	ကိန်းပြည့် status = 0;
	command_done = 0;
	do
	{
		var_na* result = collect_token(script);
		အကယ်၍(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} နေစဉ်(0 == command_done);

	အကယ်၍(0 > i) goto main_loop;

	fputs(" +> ", stdout);
	ကိန်းပြည့် j;
	အတွက်(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	var_na* program = tokens[0];
	အကယ်၍(NULL == program) exit(EXIT_FAILURE);

	ကိန်းပြည့် f = fork();
	အကယ်၍ (f == -1) exit(EXIT_FAILURE);
	မဟုတ်ပါက အကယ်၍ (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	အကယ်၍(0 == status) goto main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
