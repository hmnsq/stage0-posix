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

অক্ষর** tokens;
পূর্ণসংখ্যা command_done;

/* Function for purging line comments */
শূন্য collect_comment(FILE* প্রবেশ)
{
	পূর্ণসংখ্যা c;
	করো
	{
		c = fgetc(প্রবেশ);
		যদি(EOF == c) exit(EXIT_FAILURE);
	} যখন('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
পূর্ণসংখ্যা collect_string(FILE* প্রবেশ, পূর্ণসংখ্যা index, অক্ষর* target)
{
	পূর্ণসংখ্যা c;
collect_string_loop:
	c = fgetc(প্রবেশ);
	যদি(EOF == c) exit(EXIT_FAILURE);
	অন্যথা যদি('"' == c) ফেরত index;
	target[index] = c;
	index = index + 1;
	যাও collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
অক্ষর* collect_token(FILE* প্রবেশ)
{
	অক্ষর* token = calloc(max_string, আকার(অক্ষর));
	পূর্ণসংখ্যা c;
	পূর্ণসংখ্যা i = 0;
collect_token_loop:
	c = fgetc(প্রবেশ);
	যদি(EOF == c) exit(EXIT_SUCCESS);
	অন্যথা যদি((' ' == c) || ('\t' == c)) যাও collect_token_done;
	অন্যথা যদি('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		যাও collect_token_done;
	}
	অন্যথা যদি('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(প্রবেশ, i, token);
		যাও collect_token_done;
	}
	অন্যথা যদি('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(প্রবেশ);
		command_done = 1;
		যাও collect_token_done;
	}
	অন্যথা যদি('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(প্রবেশ);
		যাও collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	যাও collect_token_loop;

collect_token_done:
	যদি(0 == i) ফেরত ফাঁকা;
	ফেরত token;
}

পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর** argv, অক্ষর** envp)
{
	অক্ষর* filename = "kaem.run";
	FILE* script = ফাঁকা;

	যদি(ফাঁকা != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	যদি(ফাঁকা == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, আকার(অক্ষর*));

	পূর্ণসংখ্যা i = 0;
	পূর্ণসংখ্যা status = 0;
	command_done = 0;
	করো
	{
		অক্ষর* ফলাফল = collect_token(script);
		যদি(0 != ফলাফল)
		{ /* Not a comment string but an actual argument */
			tokens[i] = ফলাফল;
			i = i + 1;
		}
	} যখন(0 == command_done);

	যদি(0 > i) যাও main_loop;

	fputs(" +> ", stdout);
	পূর্ণসংখ্যা j;
	জন্য(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	অক্ষর* program = tokens[0];
	যদি(ফাঁকা == program) exit(EXIT_FAILURE);

	পূর্ণসংখ্যা f = fork();
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

	যদি(0 == status) যাও main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
