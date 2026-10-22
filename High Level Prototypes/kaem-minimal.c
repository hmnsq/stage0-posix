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

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

எழுத்து** tokens;
முழு command_done;

/* Function for purging line comments */
வெறுமை collect_comment(FILE* உள்ளீடு)
{
	முழு c;
	செய்
	{
		c = fgetc(உள்ளீடு);
		எனில்(EOF == c) exit(EXIT_FAILURE);
	} போது('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
முழு collect_string(FILE* உள்ளீடு, முழு index, எழுத்து* target)
{
	முழு c;
collect_string_loop:
	c = fgetc(உள்ளீடு);
	எனில்(EOF == c) exit(EXIT_FAILURE);
	இல்லையெனில் எனில்('"' == c) திரும்பு index;
	target[index] = c;
	index = index + 1;
	செல் collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
எழுத்து* collect_token(FILE* உள்ளீடு)
{
	எழுத்து* token = calloc(max_string, அளவு(எழுத்து));
	முழு c;
	முழு i = 0;
collect_token_loop:
	c = fgetc(உள்ளீடு);
	எனில்(EOF == c) exit(EXIT_SUCCESS);
	இல்லையெனில் எனில்((' ' == c) || ('\t' == c)) செல் collect_token_done;
	இல்லையெனில் எனில்('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		செல் collect_token_done;
	}
	இல்லையெனில் எனில்('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(உள்ளீடு, i, token);
		செல் collect_token_done;
	}
	இல்லையெனில் எனில்('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(உள்ளீடு);
		command_done = 1;
		செல் collect_token_done;
	}
	இல்லையெனில் எனில்('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(உள்ளீடு);
		செல் collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	செல் collect_token_loop;

collect_token_done:
	எனில்(0 == i) திரும்பு வெற்று;
	திரும்பு token;
}

முழு முதன்மை(முழு argc, எழுத்து** argv, எழுத்து** envp)
{
	எழுத்து* filename = "kaem.run";
	FILE* script = வெற்று;

	எனில்(வெற்று != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	எனில்(வெற்று == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, அளவு(எழுத்து*));

	முழு i = 0;
	முழு status = 0;
	command_done = 0;
	செய்
	{
		எழுத்து* result = collect_token(script);
		எனில்(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} போது(0 == command_done);

	எனில்(0 > i) செல் main_loop;

	fputs(" +> ", stdout);
	முழு j;
	க்கு(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	எழுத்து* program = tokens[0];
	எனில்(வெற்று == program) exit(EXIT_FAILURE);

	முழு f = fork();
	எனில் (f == -1) exit(EXIT_FAILURE);
	இல்லையெனில் எனில் (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	எனில்(0 == status) செல் main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
