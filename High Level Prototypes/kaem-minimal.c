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

#شامل <stdlib.h>
#شامل <stdio.h>
#شامل <unistd.h>
#شامل <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

نویسه** tokens;
صحیح command_done;

/* Function for purging line comments */
تهی collect_comment(FILE* درون_داد)
{
	صحیح c;
	انجامبده
	{
		c = fgetc(درون_داد);
		اگر(EOF == c) exit(EXIT_FAILURE);
	} وقتی('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
صحیح collect_string(FILE* درون_داد, صحیح index, نویسه* target)
{
	صحیح c;
collect_string_loop:
	c = fgetc(درون_داد);
	اگر(EOF == c) exit(EXIT_FAILURE);
	وگرنه اگر('"' == c) بازگشت index;
	target[index] = c;
	index = index + 1;
	برو collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
نویسه* collect_token(FILE* درون_داد)
{
	نویسه* token = calloc(max_string, اندازه(نویسه));
	صحیح c;
	صحیح i = 0;
collect_token_loop:
	c = fgetc(درون_داد);
	اگر(EOF == c) exit(EXIT_SUCCESS);
	وگرنه اگر((' ' == c) || ('\t' == c)) برو collect_token_done;
	وگرنه اگر('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		برو collect_token_done;
	}
	وگرنه اگر('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(درون_داد, i, token);
		برو collect_token_done;
	}
	وگرنه اگر('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(درون_داد);
		command_done = 1;
		برو collect_token_done;
	}
	وگرنه اگر('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(درون_داد);
		برو collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	برو collect_token_loop;

collect_token_done:
	اگر(0 == i) بازگشت پوچ;
	بازگشت token;
}

صحیح اصلی(صحیح argc, نویسه** argv, نویسه** envp)
{
	نویسه* filename = "kaem.run";
	FILE* script = پوچ;

	اگر(پوچ != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	اگر(پوچ == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, اندازه(نویسه*));

	صحیح i = 0;
	صحیح status = 0;
	command_done = 0;
	انجامبده
	{
		نویسه* result = collect_token(script);
		اگر(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} وقتی(0 == command_done);

	اگر(0 > i) برو main_loop;

	fputs(" +> ", stdout);
	صحیح j;
	برای(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	نویسه* program = tokens[0];
	اگر(پوچ == program) exit(EXIT_FAILURE);

	صحیح f = fork();
	اگر (f == -1) exit(EXIT_FAILURE);
	وگرنه اگر (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	اگر(0 == status) برو main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
