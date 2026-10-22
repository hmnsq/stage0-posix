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

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

বর্ণ** tokens;
পূর্ণাঙ্ক command_done;

/* Function for purging line comments */
শূন্য collect_comment(FILE* প্রবেশ)
{
	পূর্ণাঙ্ক c;
	কুরু
	{
		c = fgetc(প্রবেশ);
		যদি(EOF == c) exit(EXIT_FAILURE);
	} যাবত্('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
পূর্ণাঙ্ক collect_string(FILE* প্রবেশ, পূর্ণাঙ্ক index, বর্ণ* target)
{
	পূর্ণাঙ্ক c;
collect_string_loop:
	c = fgetc(প্রবেশ);
	যদি(EOF == c) exit(EXIT_FAILURE);
	অন্যথা যদি('"' == c) প্রত্যাগম index;
	target[index] = c;
	index = index + 1;
	গচ্ছ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
বর্ণ* collect_token(FILE* প্রবেশ)
{
	বর্ণ* token = calloc(max_string, পরিমাণ(বর্ণ));
	পূর্ণাঙ্ক c;
	পূর্ণাঙ্ক i = 0;
collect_token_loop:
	c = fgetc(প্রবেশ);
	যদি(EOF == c) exit(EXIT_SUCCESS);
	অন্যথা যদি((' ' == c) || ('\t' == c)) গচ্ছ collect_token_done;
	অন্যথা যদি('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		গচ্ছ collect_token_done;
	}
	অন্যথা যদি('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(প্রবেশ, i, token);
		গচ্ছ collect_token_done;
	}
	অন্যথা যদি('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(প্রবেশ);
		command_done = 1;
		গচ্ছ collect_token_done;
	}
	অন্যথা যদি('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(প্রবেশ);
		গচ্ছ collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	গচ্ছ collect_token_loop;

collect_token_done:
	যদি(0 == i) প্রত্যাগম রিক্ত;
	প্রত্যাগম token;
}

পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, বর্ণ** argv, বর্ণ** envp)
{
	বর্ণ* filename = "kaem.run";
	FILE* script = রিক্ত;

	যদি(রিক্ত != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	যদি(রিক্ত == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, পরিমাণ(বর্ণ*));

	পূর্ণাঙ্ক i = 0;
	পূর্ণাঙ্ক status = 0;
	command_done = 0;
	কুরু
	{
		বর্ণ* result = collect_token(script);
		যদি(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} যাবত্(0 == command_done);

	যদি(0 > i) গচ্ছ main_loop;

	fputs(" +> ", stdout);
	পূর্ণাঙ্ক j;
	প্রতি(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	বর্ণ* program = tokens[0];
	যদি(রিক্ত == program) exit(EXIT_FAILURE);

	পূর্ণাঙ্ক f = fork();
	যদি (f == -1) exit(EXIT_FAILURE);
	অন্যথা যদি (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	যদি(0 == status) গচ্ছ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
