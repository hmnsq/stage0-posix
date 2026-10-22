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

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

അക്ഷരം** tokens;
പൂർണ്ണസംഖ്യ command_done;

/* Function for purging line comments */
ശൂന്യം collect_comment(FILE* നിവേശം)
{
	പൂർണ്ണസംഖ്യ c;
	ചെയ്യുക
	{
		c = fgetc(നിവേശം);
		എങ്കിൽ(EOF == c) exit(EXIT_FAILURE);
	} ആകുമ്പോൾ('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
പൂർണ്ണസംഖ്യ collect_string(FILE* നിവേശം, പൂർണ്ണസംഖ്യ index, അക്ഷരം* target)
{
	പൂർണ്ണസംഖ്യ c;
collect_string_loop:
	c = fgetc(നിവേശം);
	എങ്കിൽ(EOF == c) exit(EXIT_FAILURE);
	അല്ലെങ്കിൽ എങ്കിൽ('"' == c) തിരികെ index;
	target[index] = c;
	index = index + 1;
	പോകുക collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
അക്ഷരം* collect_token(FILE* നിവേശം)
{
	അക്ഷരം* token = calloc(max_string, വലുപ്പം(അക്ഷരം));
	പൂർണ്ണസംഖ്യ c;
	പൂർണ്ണസംഖ്യ i = 0;
collect_token_loop:
	c = fgetc(നിവേശം);
	എങ്കിൽ(EOF == c) exit(EXIT_SUCCESS);
	അല്ലെങ്കിൽ എങ്കിൽ((' ' == c) || ('\t' == c)) പോകുക collect_token_done;
	അല്ലെങ്കിൽ എങ്കിൽ('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		പോകുക collect_token_done;
	}
	അല്ലെങ്കിൽ എങ്കിൽ('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(നിവേശം, i, token);
		പോകുക collect_token_done;
	}
	അല്ലെങ്കിൽ എങ്കിൽ('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(നിവേശം);
		command_done = 1;
		പോകുക collect_token_done;
	}
	അല്ലെങ്കിൽ എങ്കിൽ('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(നിവേശം);
		പോകുക collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	പോകുക collect_token_loop;

collect_token_done:
	എങ്കിൽ(0 == i) തിരികെ ശൂന്യ;
	തിരികെ token;
}

പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം** argv, അക്ഷരം** envp)
{
	അക്ഷരം* filename = "kaem.run";
	FILE* script = ശൂന്യ;

	എങ്കിൽ(ശൂന്യ != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	എങ്കിൽ(ശൂന്യ == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, വലുപ്പം(അക്ഷരം*));

	പൂർണ്ണസംഖ്യ i = 0;
	പൂർണ്ണസംഖ്യ status = 0;
	command_done = 0;
	ചെയ്യുക
	{
		അക്ഷരം* result = collect_token(script);
		എങ്കിൽ(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} ആകുമ്പോൾ(0 == command_done);

	എങ്കിൽ(0 > i) പോകുക main_loop;

	fputs(" +> ", stdout);
	പൂർണ്ണസംഖ്യ j;
	വേണ്ടി(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	അക്ഷരം* program = tokens[0];
	എങ്കിൽ(ശൂന്യ == program) exit(EXIT_FAILURE);

	പൂർണ്ണസംഖ്യ f = fork();
	എങ്കിൽ (f == -1) exit(EXIT_FAILURE);
	അല്ലെങ്കിൽ എങ്കിൽ (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	എങ്കിൽ(0 == status) പോകുക main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
