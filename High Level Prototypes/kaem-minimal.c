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
#define max_حجوجي 256

حرف** tokens;
صحيح command_done;

/* Function for purging line comments */
فراغ collect_comment(FILE* شغرو)
{
	صحيح c;
	افعل
	{
		c = fgetc(شغرو);
		إذا(EOF == c) exit(EXIT_FAILURE);
	} طالما('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
صحيح collect_string(FILE* شغرو, صحيح index, حرف* target)
{
	صحيح c;
collect_string_loop:
	c = fgetc(شغرو);
	إذا(EOF == c) exit(EXIT_FAILURE);
	وإلا إذا('"' == c) أرجع index;
	target[index] = c;
	index = index + 1;
	اذهب collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
حرف* collect_token(FILE* شغرو)
{
	حرف* token = calloc(max_string, حجم(حرف));
	صحيح c;
	صحيح i = 0;
collect_token_loop:
	c = fgetc(شغرو);
	إذا(EOF == c) exit(EXIT_SUCCESS);
	وإلا إذا((' ' == c) || ('\t' == c)) اذهب collect_token_done;
	وإلا إذا('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		اذهب collect_token_done;
	}
	وإلا إذا('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(شغرو, i, token);
		اذهب collect_token_done;
	}
	وإلا إذا('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(شغرو);
		command_done = 1;
		اذهب collect_token_done;
	}
	وإلا إذا('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(شغرو);
		اذهب collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	اذهب collect_token_loop;

collect_token_done:
	إذا(0 == i) أرجع عدم;
	أرجع token;
}

صحيح main(صحيح argc, حرف** argv, حرف** envp)
{
	حرف* filename = "kaem.run";
	FILE* script = عدم;

	إذا(عدم != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	إذا(عدم == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_حجوجي, حجم(حرف*));

	صحيح i = 0;
	صحيح status = 0;
	command_done = 0;
	افعل
	{
		حرف* result = collect_token(script);
		إذا(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} طالما(0 == command_done);

	إذا(0 > i) اذهب main_loop;

	fputs(" +> ", stdout);
	صحيح j;
	لكل(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	حرف* program = tokens[0];
	إذا(عدم == program) exit(EXIT_FAILURE);

	صحيح f = fork();
	إذا (f == -1) exit(EXIT_FAILURE);
	وإلا إذا (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	إذا(0 == status) اذهب main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
