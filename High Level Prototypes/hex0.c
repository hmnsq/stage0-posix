/* Copyright (C) 2020 Jeremiah Orians
 * Copyright (C) 2017 Jan Nieuwenhuizen <janneke@gnu.org>
 * This file is part of stage0
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

#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <unistd.h>
#समावेश <sys/stat.h>
#समावेश "../M2libc/bootstrappable.h"

#घोषणा max_string 4096

/* Globals */
खाता* output;

व्योम पंक्ति_Comment(खाता* source_file)
{
	पूर्णांक c = ख_अक्षर_लो(source_file);
	जबतक(!in_set(c, "\n\r"))
	{
		अगर(खातापूर्ण == c) अवरोध;
		c = ख_अक्षर_लो(source_file);
	}
}

पूर्णांक hex(पूर्णांक c, खाता* source_file)
{
	अगर (in_set(c, "0123456789")) वापस (c - 48);
	अन्यथा अगर (in_set(c, "abcdef")) वापस (c - 87);
	अन्यथा अगर (in_set(c, "ABCDEF")) वापस (c - 55);
	अन्यथा अगर (in_set(c, "#;")) पंक्ति_Comment(source_file);
	वापस -1;
}

पूर्णांक hold;
पूर्णांक toggle;
व्योम process_byte(अक्षर c, खाता* source_file)
{
	अगर(0 <= hex(c, source_file))
	{
		अगर(toggle)
		{
			ख_अक्षर_दो(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		अन्यथा
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

व्योम first_pass(खाता* निवेश)
{
	toggle = FALSE;
	पूर्णांक c;
	क्रम(c = ख_अक्षर_लो(निवेश); खातापूर्ण != c; c = ख_अक्षर_लो(निवेश))
	{
		process_byte(c, निवेश);
	}
}

/* Standard C main program */
पूर्णांक मुख्य(पूर्णांक argc, अक्षर **argv)
{
	output = मानक_निकास;

	खाता* निवेश = ख_खोलो(argv[1], "r");
	output = मानक_निकास;
	अगर(शून्य != argv[2])
	{
		output = ख_खोलो(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(निवेश);

	/* Set file as executable */
	वापस निकास_सफल;
}
