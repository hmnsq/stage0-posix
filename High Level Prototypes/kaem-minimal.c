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

#define FALSE 0
#define TRUE 1
#define max_string 4096
#define max_args 256

वर्ण** tokens;
पूर्णाङ्क command_done;

/* Function for purging line comments */
शून्य collect_comment(खाता* आगत)
{
	पूर्णाङ्क c;
	गर
	{
		c = ख_अक्षर_लो(आगत);
		यदि(खातापूर्ण == c) निकास(निकास_त्रुटि);
	} जबसम्म('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
पूर्णाङ्क collect_string(खाता* आगत, पूर्णाङ्क index, वर्ण* target)
{
	पूर्णाङ्क c;
collect_string_loop:
	c = ख_अक्षर_लो(आगत);
	यदि(खातापूर्ण == c) निकास(निकास_त्रुटि);
	अन्यथा यदि('"' == c) फर्काऊ index;
	target[index] = c;
	index = index + 1;
	गच्छ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
वर्ण* collect_token(खाता* आगत)
{
	वर्ण* token = सुस्मृति(max_string, परिमाण(वर्ण));
	पूर्णाङ्क c;
	पूर्णाङ्क i = 0;
collect_token_loop:
	c = ख_अक्षर_लो(आगत);
	यदि(खातापूर्ण == c) निकास(निकास_सफल);
	अन्यथा यदि((' ' == c) || ('\t' == c)) गच्छ collect_token_done;
	अन्यथा यदि('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		गच्छ collect_token_done;
	}
	अन्यथा यदि('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(आगत, i, token);
		गच्छ collect_token_done;
	}
	अन्यथा यदि('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(आगत);
		command_done = 1;
		गच्छ collect_token_done;
	}
	अन्यथा यदि('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		ख_अक्षर_लो(आगत);
		गच्छ collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	गच्छ collect_token_loop;

collect_token_done:
	यदि(0 == i) फर्काऊ रिक्त;
	फर्काऊ token;
}

पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण** argv, वर्ण** envp)
{
	वर्ण* filename = "kaem.run";
	खाता* script = रिक्त;

	यदि(रिक्त != argv[1])
	{
		filename = argv[1];
	}

	script = ख_खोलो(filename, "r");

	यदि(रिक्त == script) निकास(निकास_त्रुटि);

main_loop:
	tokens = सुस्मृति(max_args, परिमाण(वर्ण*));

	पूर्णाङ्क i = 0;
	पूर्णाङ्क status = 0;
	command_done = 0;
	गर
	{
		वर्ण* result = collect_token(script);
		यदि(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} जबसम्म(0 == command_done);

	यदि(0 > i) गच्छ main_loop;

	ख_माला_दो(" +> ", मानक_निकास);
	पूर्णाङ्क j;
	लागि(j = 0; j < i; j = j + 1)
	{
		ख_माला_दो(tokens[j], मानक_निकास);
		ख_अक्षर_दो(' ', मानक_निकास);
	}
	ख_माला_दो("\n", मानक_निकास);

	वर्ण* program = tokens[0];
	यदि(रिक्त == program) निकास(निकास_त्रुटि);

	पूर्णाङ्क f = विभाजन();
	यदि (f == -1) निकास(निकास_त्रुटि);
	अन्यथा यदि (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		निकास(निकास_त्रुटि);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	यदि(0 == status) गच्छ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	ख_माला_दो("Subprocess error\nABORTING HARD\n", मानक_त्रुटि);
	/* stop to prevent damage */
	निकास(निकास_त्रुटि);
}
