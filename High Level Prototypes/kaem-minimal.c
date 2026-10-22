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

#समावेश <stdlib.h>
#समावेश <stdio.h>
#समावेश <unistd.h>
#समावेश <sys/wait.h>

#घोषणा FALSE 0
#घोषणा TRUE 1
#घोषणा max_string 4096
#घोषणा max_args 256

अक्षर** tokens;
पूर्णांक command_done;

/* Function for purging line comments */
व्योम collect_comment(खाता* निवेश)
{
	पूर्णांक c;
	करो
	{
		c = ख_अक्षर_लो(निवेश);
		अगर(खातापूर्ण == c) निकास(निकास_त्रुटि);
	} जबतक('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
पूर्णांक collect_string(खाता* निवेश, पूर्णांक index, अक्षर* target)
{
	पूर्णांक c;
collect_string_loop:
	c = ख_अक्षर_लो(निवेश);
	अगर(खातापूर्ण == c) निकास(निकास_त्रुटि);
	अन्यथा अगर('"' == c) वापस index;
	target[index] = c;
	index = index + 1;
	जाओ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
अक्षर* collect_token(खाता* निवेश)
{
	अक्षर* token = सुस्मृति(max_string, माप(अक्षर));
	पूर्णांक c;
	पूर्णांक i = 0;
collect_token_loop:
	c = ख_अक्षर_लो(निवेश);
	अगर(खातापूर्ण == c) निकास(निकास_सफल);
	अन्यथा अगर((' ' == c) || ('\t' == c)) जाओ collect_token_done;
	अन्यथा अगर('\n' == c)
	{ /* Command terminates at end of line */
		command_done = 1;
		जाओ collect_token_done;
	}
	अन्यथा अगर('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(निवेश, i, token);
		जाओ collect_token_done;
	}
	अन्यथा अगर('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(निवेश);
		command_done = 1;
		जाओ collect_token_done;
	}
	अन्यथा अगर('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		ख_अक्षर_लो(निवेश);
		जाओ collect_token_done;
	}
	token[i] = c;
	i = i + 1;
	जाओ collect_token_loop;

collect_token_done:
	अगर(0 == i) वापस शून्य;
	वापस token;
}

पूर्णांक मुख्य(पूर्णांक argc, अक्षर** argv, अक्षर** envp)
{
	अक्षर* filename = "kaem.run";
	खाता* script = शून्य;

	अगर(शून्य != argv[1])
	{
		filename = argv[1];
	}

	script = ख_खोलो(filename, "r");

	अगर(शून्य == script) निकास(निकास_त्रुटि);

main_loop:
	tokens = सुस्मृति(max_args, माप(अक्षर*));

	पूर्णांक i = 0;
	पूर्णांक status = 0;
	command_done = 0;
	करो
	{
		अक्षर* परिणाम = collect_token(script);
		अगर(0 != परिणाम)
		{ /* Not a comment string but an actual argument */
			tokens[i] = परिणाम;
			i = i + 1;
		}
	} जबतक(0 == command_done);

	अगर(0 > i) जाओ main_loop;

	ख_माला_दो(" +> ", मानक_निकास);
	पूर्णांक j;
	क्रम(j = 0; j < i; j = j + 1)
	{
		ख_माला_दो(tokens[j], मानक_निकास);
		ख_अक्षर_दो(' ', मानक_निकास);
	}
	ख_माला_दो("\n", मानक_निकास);

	अक्षर* program = tokens[0];
	अगर(शून्य == program) निकास(निकास_त्रुटि);

	पूर्णांक f = विभाजन();
	अगर (f == -1) निकास(निकास_त्रुटि);
	अन्यथा अगर (f == 0)
	{ /* child */
		/* execve() returns only on error */
		execve(program, tokens, envp);
		/* Prevent infinite loops */
		निकास(निकास_त्रुटि);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &status, 0);

	अगर(0 == status) जाओ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	ख_माला_दो("Subprocess error\nABORTING HARD\n", मानक_त्रुटि);
	/* stop to prevent damage */
	निकास(निकास_त्रुटि);
}
