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

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

ವರ್ಣ** tokens;
ಪೂರ್ಣಾಙ್ಕ command_done;

/* Function for purging line comments */
ಶೂನ್ಯ collect_comment(FILE* ಒಳಹರಿವು)
{
	ಪೂರ್ಣಾಙ್ಕ c;
	ಕುರು
	{
		c = fgetc(ಒಳಹರಿವು);
		ಯದಿ(EOF == c) exit(EXIT_FAILURE);
	} ಯಾವತ್('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
ಪೂರ್ಣಾಙ್ಕ collect_string(FILE* ಒಳಹರಿವು, ಪೂರ್ಣಾಙ್ಕ index, ವರ್ಣ* target)
{
	ಪೂರ್ಣಾಙ್ಕ c;
collect_string_loop:
	c = fgetc(ಒಳಹರಿವು);
	ಯದಿ(EOF == c) exit(EXIT_FAILURE);
	ಅನ್ಯಥಾ ಯದಿ('"' == c) ಪ್ರತ್ಯಾಗಮ index;
	target[index] = c;
	index = index + 1;
	ಗಚ್ಛ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
ವರ್ಣ* collect_token(FILE* ಒಳಹರಿವು)
{
	ವರ್ಣ* token = calloc(max_string, ಪರಿಮಾಣ(ವರ್ಣ));
	ಪೂರ್ಣಾಙ್ಕ c;
	ಪೂರ್ಣಾಙ್ಕ i = 0;
collect_token_loop:
	c = fgetc(ಒಳಹರಿವು);
	ಯದಿ(EOF == c) exit(EXIT_SUCCESS);
	ಅನ್ಯಥಾ ಯದಿ((' ' == c) || ('\t' == c)) ಗಚ್ಛ collect_token_done;
	ಅನ್ಯಥಾ ಯದಿ('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		ಗಚ್ಛ collect_token_done;
	}
	ಅನ್ಯಥಾ ಯದಿ('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(ಒಳಹರಿವು, i, token);
		ಗಚ್ಛ collect_token_done;
	}
	ಅನ್ಯಥಾ ಯದಿ('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(ಒಳಹರಿವು);
		command_done = 1;
		ಗಚ್ಛ collect_token_done;
	}
	ಅನ್ಯಥಾ ಯದಿ('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(ಒಳಹರಿವು);
		ಗಚ್ಛ collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	ಗಚ್ಛ collect_token_loop;

collect_token_done:
	ಯದಿ(0 == i) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ token;
}

ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ** argv, ವರ್ಣ** envp)
{
	ವರ್ಣ* filename = "kaem.run";
	FILE* script = ರಿಕ್ತ;

	ಯದಿ(ರಿಕ್ತ != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	ಯದಿ(ರಿಕ್ತ == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, ಪರಿಮಾಣ(ವರ್ಣ*));

	ಪೂರ್ಣಾಙ್ಕ i = 0;
	ಪೂರ್ಣಾಙ್ಕ status = 0;
	command_done = 0;
	ಕುರು
	{
		ವರ್ಣ* result = collect_token(script);
		ಯದಿ(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} ಯಾವತ್(0 == command_done);

	ಯದಿ(0 > i) ಗಚ್ಛ main_loop;

	fputs(" +> ", stdout);
	ಪೂರ್ಣಾಙ್ಕ j;
	ಪ್ರತಿ(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	ವರ್ಣ* program = tokens[0];
	ಯದಿ(ರಿಕ್ತ == program) exit(EXIT_FAILURE);

	ಪೂರ್ಣಾಙ್ಕ f = fork();
	ಯದಿ (f == -1) exit(EXIT_FAILURE);
	ಅನ್ಯಥಾ ಯದಿ (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	ಯದಿ(0 == status) ಗಚ್ಛ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
