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

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

ଅକ୍ଷର** tokens;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା command_done;

/* Function for purging line comments */
ଶୂନ୍ୟ collect_comment(FILE* ନିବେଶ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	କର
	{
		c = fgetc(ନିବେଶ);
		ଯଦି(EOF == c) exit(EXIT_FAILURE);
	} ଯେତେବେଳେ('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା collect_string(FILE* ନିବେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ଅକ୍ଷର* target)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
collect_string_loop:
	c = fgetc(ନିବେଶ);
	ଯଦି(EOF == c) exit(EXIT_FAILURE);
	ଅନ୍ୟଥା ଯଦି('"' == c) ଫେରନ୍ତୁ index;
	target[index] = c;
	index = index + 1;
	ଯାଅ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
ଅକ୍ଷର* collect_token(FILE* ନିବେଶ)
{
	ଅକ୍ଷର* token = calloc(max_string, ଆକାର(ଅକ୍ଷର));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
collect_token_loop:
	c = fgetc(ନିବେଶ);
	ଯଦି(EOF == c) exit(EXIT_SUCCESS);
	ଅନ୍ୟଥା ଯଦି((' ' == c) || ('\t' == c)) ଯାଅ collect_token_done;
	ଅନ୍ୟଥା ଯଦି('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		ଯାଅ collect_token_done;
	}
	ଅନ୍ୟଥା ଯଦି('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(ନିବେଶ, i, token);
		ଯାଅ collect_token_done;
	}
	ଅନ୍ୟଥା ଯଦି('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(ନିବେଶ);
		command_done = 1;
		ଯାଅ collect_token_done;
	}
	ଅନ୍ୟଥା ଯଦି('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(ନିବେଶ);
		ଯାଅ collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	ଯାଅ collect_token_loop;

collect_token_done:
	ଯଦି(0 == i) ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ token;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର** argv, ଅକ୍ଷର** envp)
{
	ଅକ୍ଷର* filename = "kaem.run";
	FILE* script = ଖାଲି;

	ଯଦି(ଖାଲି != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	ଯଦି(ଖାଲି == script) exit(EXIT_FAILURE);

main_loop:
	tokens = calloc(max_args, ଆକାର(ଅକ୍ଷର*));

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = 0;
	command_done = 0;
	କର
	{
		ଅକ୍ଷର* result = collect_token(script);
		ଯଦି(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} ଯେତେବେଳେ(0 == command_done);

	ଯଦି(0 > i) ଯାଅ main_loop;

	fputs(" +> ", stdout);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;
	ପାଇଁ(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	ଅକ୍ଷର* program = tokens[0];
	ଯଦି(ଖାଲି == program) exit(EXIT_FAILURE);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା f = fork();
	ଯଦି (f == -1) exit(EXIT_FAILURE);
	ଅନ୍ୟଥା ଯଦି (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		exit(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	ଯଦି(0 == status) ଯାଅ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	exit(EXIT_FAILURE);
}
