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

अक्षर** tokens;
पूर्णांक command_done;

/* Function for purging line comments */
शून्य collect_comment(खाता* आदान)
{
	पूर्णांक c;
	करा
	{
		c = ख_अक्षर_लो(आदान);
		जर(खातापूर्ण == c) निकास(निकास_त्रुटि);
	} पर्यंत('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
पूर्णांक collect_string(खाता* आदान, पूर्णांक index, अक्षर* target)
{
	पूर्णांक c;
collect_string_loop:
	c = ख_अक्षर_लो(आदान);
	जर(खातापूर्ण == c) निकास(निकास_त्रुटि);
	अन्यथा जर('"' == c) परत index;
	target[index] = c;
	index = index + 1;
	जा collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
अक्षर* collect_token(खाता* आदान)
{
	अक्षर* token = सुस्मृति(max_string, आकार(अक्षर));
	पूर्णांक c;
	पूर्णांक i = 0;
collect_token_loop:
	c = ख_अक्षर_लो(आदान);
	जर(खातापूर्ण == c) निकास(निकास_सफल);
	अन्यथा जर((' ' == c) || ('\t' == c)) जा collect_token_done;
	अन्यथा जर('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		जा collect_token_done;
	}
	अन्यथा जर('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(आदान, i, token);
		जा collect_token_done;
	}
	अन्यथा जर('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(आदान);
		command_done = 1;
		जा collect_token_done;
	}
	अन्यथा जर('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		ख_अक्षर_लो(आदान);
		जा collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	जा collect_token_loop;

collect_token_done:
	जर(0 == i) परत रिक्त;
	परत token;
}

पूर्णांक main(पूर्णांक argc, अक्षर** argv, अक्षर** envp)
{
	अक्षर* filename = "kaem.run";
	खाता* script = रिक्त;

	जर(रिक्त != argv[1])
	{
		filename = argv[1];
	}

	script = ख_खोलो(filename, "r");

	जर(रिक्त == script) निकास(निकास_त्रुटि);

main_loop:
	tokens = सुस्मृति(max_args, आकार(अक्षर*));

	पूर्णांक i = 0;
	पूर्णांक status = 0;
	command_done = 0;
	करा
	{
		अक्षर* result = collect_token(script);
		जर(0 != result)
		{ /* Not a comment string but an actual argument */
			tokens[i] = result;
			i = i + 1;
		}
	} पर्यंत(0 == command_done);

	जर(0 > i) जा main_loop;

	ख_माला_दो(" +> ", मानक_निकास);
	पूर्णांक j;
	साठी(j = 0; j < i; j = j + 1)
	{
		ख_माला_दो(tokens[j], मानक_निकास);
		ख_अक्षर_दो(' ', मानक_निकास);
	}
	ख_माला_दो("\n", मानक_निकास);

	अक्षर* program = tokens[0];
	जर(रिक्त == program) निकास(निकास_त्रुटि);

	पूर्णांक f = विभाजन();
	जर (f == -1) निकास(निकास_त्रुटि);
	अन्यथा जर (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		निकास(निकास_त्रुटि);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	जर(0 == status) जा main_loop;

	/* Clearly the script hit an issue that should never have happened */
	ख_माला_दो("Subprocess error\nABORTING HARD\n", मानक_त्रुटि);
	/* stop to prevent damage */
	निकास(निकास_त्रुटि);
}
