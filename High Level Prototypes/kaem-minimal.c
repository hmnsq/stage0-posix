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

#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

અક્ષર** tokens;
પૂર્ણાંક command_done;

/* Function for purging line comments */
શૂન્ય collect_comment(FILE* આગત)
{
	પૂર્ણાંક c;
	કરો
	{
		c = fgetc(આગત);
		જો(EOF == c) exit(EXIT_FAILURE);
	} જ્યારે('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
પૂર્ણાંક collect_string(FILE* આગત, પૂર્ણાંક index, અક્ષર* target)
{
	પૂર્ણાંક c;
collect_string_loop:
	c = fgetc(આગત);
	જો(EOF == c) exit(EXIT_FAILURE);
	નહીંતર જો('"' == c) પાછા index;
	target[index] = c;
	index = index + 1;
	જાઓ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
અક્ષર* collect_token(FILE* આગત)
{
	અક્ષર* token = calloc(max_string, કદ(અક્ષર));
	પૂર્ણાંક c;
	પૂર્ણાંક i = 0;
collect_token_loop:
	c = fgetc(આગત);
	જો(EOF == c) exit(EXIT_SUCCESS);
	નહીંતર જો((' ' == c) || ('\t' == c)) જાઓ collect_token_done;
	નહીંતર જો('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		જાઓ collect_token_done;
	}
	નહીંતર જો('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(આગત, i, token);
		જાઓ collect_token_done;
	}
	નહીંતર જો('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(આગત);
		command_done = 1;
		જાઓ collect_token_done;
	}
	નહીંતર જો('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(આગત);
		જાઓ collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	જાઓ collect_token_loop;

collect_token_done:
	જો(0 == i) પાછા ખાલી;
	પાછા token;
}

પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર** argv, અક્ષર** envp)
{
	અક્ષર* filename = "kaem.run";
	FILE* script = ખાલી;

	જો(ખાલી != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	જો(ખાલી == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, કદ(અક્ષર*));

	પૂર્ણાંક i = 0;
	પૂર્ણાંક status = 0;
	command_done = 0;
	કરો
	{
		અક્ષર* result = collect_token(script);
		જો(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} જ્યારે(0 == command_done);

	જો(0 > i) જાઓ main_loop;

	fputs(" +> ", stdout);
	પૂર્ણાંક j;
	માટે(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	અક્ષર* program = tokens[0];
	જો(ખાલી == program) exit(EXIT_FAILURE);

	પૂર્ણાંક f = fork();
	જો (f == -1) exit(EXIT_FAILURE);
	નહીંતર જો (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	જો(0 == status) જાઓ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
