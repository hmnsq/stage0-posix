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

#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن <unistd.h>
#ضمن <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

حرف** tokens;
صحيح command_done;

/* Function for purging line comments */
فراغ collect_comment(FILE* input)
{
	صحيح c;
	ܥܒܕ
	{
		c = fgetc(input);
		ܐܢ(EOF == c) exit(EXIT_FAILURE);
	} ܟܡܐ('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
صحيح collect_string(FILE* input, صحيح index, حرف* target)
{
	صحيح c;
collect_string_loop:
	c = fgetc(input);
	ܐܢ(EOF == c) exit(EXIT_FAILURE);
	ܐܚܪܢܐ ܐܢ('"' == c) ܦܢܐ index;
	target[index] = c;
	index = index + 1;
	اذهب collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
حرف* collect_token(FILE* input)
{
	حرف* token = calloc(max_string, حجم(حرف));
	صحيح c;
	صحيح i = 0;
collect_token_loop:
	c = fgetc(input);
	ܐܢ(EOF == c) exit(EXIT_SUCCESS);
	ܐܚܪܢܐ ܐܢ((' ' == c) || ('\t' == c)) اذهب collect_token_done;
	ܐܚܪܢܐ ܐܢ('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		اذهب collect_token_done;
	}
	ܐܚܪܢܐ ܐܢ('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(input, i, token);
		اذهب collect_token_done;
	}
	ܐܚܪܢܐ ܐܢ('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(input);
		command_done = 1;
		اذهب collect_token_done;
	}
	ܐܚܪܢܐ ܐܢ('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(input);
		اذهب collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	اذهب collect_token_loop;

collect_token_done:
	ܐܢ(0 == i) ܦܢܐ عدم;
	ܦܢܐ token;
}

صحيح main(صحيح argc, حرف** argv, حرف** envp)
{
	حرف* filename = "kaem.run";
	FILE* script = عدم;

	ܐܢ(عدم != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	ܐܢ(عدم == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, حجم(حرف*));

	صحيح i = 0;
	صحيح status = 0;
	command_done = 0;
	ܥܒܕ
	{
		حرف* result = collect_token(script);
		ܐܢ(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} ܟܡܐ(0 == command_done);

	ܐܢ(0 > i) اذهب main_loop;

	fputs(" +> ", stdout);
	صحيح j;
	لكل(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	حرف* program = tokens[0];
	ܐܢ(عدم == program) exit(EXIT_FAILURE);

	صحيح f = fork();
	ܐܢ (f == -1) exit(EXIT_FAILURE);
	ܐܚܪܢܐ ܐܢ (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	ܐܢ(0 == status) اذهب main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
