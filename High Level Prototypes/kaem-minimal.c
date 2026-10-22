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

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <sys/wait.h>

#परिभाषय FALSE 0
#परिभाषय TRUE 1
#परिभाषय max_string 4096
#परिभाषय max_तर्काः 256

वर्ण** tokens;
पूर्णाङ्क command_समाप्तम्;

/* Function for purging line comments */
शून्य collect_comment(FILE* निवेशः)
{
	पूर्णाङ्क c;
	कुरु
	{
		c = fgetc(निवेशः);
		यदि(EOF == c) निर्गच्छ(EXIT_FAILURE);
	} यावत्('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
पूर्णाङ्क collect_string(FILE* निवेशः, पूर्णाङ्क index, वर्ण* target)
{
	पूर्णाङ्क c;
collect_string_loop:
	c = fgetc(निवेशः);
	यदि(EOF == c) निर्गच्छ(EXIT_FAILURE);
	अन्यथा यदि('"' == c) प्रत्यागम index;
	target[index] = c;
	index = index + 1;
	गच्छ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
वर्ण* collect_token(FILE* निवेशः)
{
	वर्ण* token = शून्यस्मृतिं_देहि(max_string, परिमाण(वर्ण));
	पूर्णाङ्क c;
	पूर्णाङ्क i = 0;
collect_token_loop:
	c = fgetc(निवेशः);
	यदि(EOF == c) निर्गच्छ(EXIT_SUCCESS);
	अन्यथा यदि((' ' == c) || ('\t' == c)) गच्छ collect_token_समाप्तम्;
	अन्यथा यदि('\n' == c)
	{ /* Command terminates at end of line */
		command_समाप्तम् = 1;
		गच्छ collect_token_समाप्तम्;
	}
	अन्यथा यदि('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(निवेशः, i, token);
		गच्छ collect_token_समाप्तम्;
	}
	अन्यथा यदि('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(निवेशः);
		command_समाप्तम् = 1;
		गच्छ collect_token_समाप्तम्;
	}
	अन्यथा यदि('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(निवेशः);
		गच्छ collect_token_समाप्तम्;
	}
	token[i] = c;
	i = i + 1;
	गच्छ collect_token_loop;

collect_token_समाप्तम्:
	यदि(0 == i) प्रत्यागम रिक्त;
	प्रत्यागम token;
}

पूर्णाङ्क मुख्यम्(पूर्णाङ्क argc, वर्ण** argv, वर्ण** envp)
{
	वर्ण* filename = "kaem.run";
	FILE* script = रिक्त;

	यदि(रिक्त != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	यदि(रिक्त == script) निर्गच्छ(EXIT_FAILURE);

main_loop:
	tokens = शून्यस्मृतिं_देहि(max_तर्काः, परिमाण(वर्ण*));

	पूर्णाङ्क i = 0;
	पूर्णाङ्क स्थितिः = 0;
	command_समाप्तम् = 0;
	कुरु
	{
		वर्ण* परिणाम = collect_token(script);
		यदि(0 != परिणाम)
		{ /* Not a comment string but an actual argument */
			tokens[i] = परिणाम;
			i = i + 1;
		}
	} यावत्(0 == command_समाप्तम्);

	यदि(0 > i) गच्छ main_loop;

	fputs(" +> ", stdout);
	पूर्णाङ्क j;
	प्रति(j = 0; j < i; j = j + 1)
	{
		fputs(tokens[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	वर्ण* क्रमादेशः = tokens[0];
	यदि(रिक्त == क्रमादेशः) निर्गच्छ(EXIT_FAILURE);

	पूर्णाङ्क f = fork();
	यदि (f == -1) निर्गच्छ(EXIT_FAILURE);
	अन्यथा यदि (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(क्रमादेशः, tokens, envp);
		/* Prevent infinite loops */
		निर्गच्छ(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &स्थितिः, 0);

	यदि(0 == स्थितिः) गच्छ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	निर्गच्छ(EXIT_FAILURE);
}
