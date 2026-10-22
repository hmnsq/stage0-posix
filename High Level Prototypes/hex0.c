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

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव "../M2libc/bootstrappable.h"

#define max_string 4096

/* Globals */
खाता* output;

शून्य ओळ_Comment(खाता* source_file)
{
	पूर्णांक c = ख_अक्षर_लो(source_file);
	पर्यंत(!in_set(c, "\n\r"))
	{
		जर(खातापूर्ण == c) थांबा;
		c = ख_अक्षर_लो(source_file);
	}
}

पूर्णांक hex(पूर्णांक c, खाता* source_file)
{
	जर (in_set(c, "0123456789")) परत (c - 48);
	अन्यथा जर (in_set(c, "abcdef")) परत (c - 87);
	अन्यथा जर (in_set(c, "ABCDEF")) परत (c - 55);
	अन्यथा जर (in_set(c, "#;")) ओळ_Comment(source_file);
	परत -1;
}

पूर्णांक hold;
पूर्णांक toggle;
शून्य process_byte(अक्षर c, खाता* source_file)
{
	जर(0 <= hex(c, source_file))
	{
		जर(toggle)
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

शून्य first_pass(खाता* आदान)
{
	toggle = FALSE;
	पूर्णांक c;
	साठी(c = ख_अक्षर_लो(आदान); खातापूर्ण != c; c = ख_अक्षर_लो(आदान))
	{
		process_byte(c, आदान);
	}
}

/* Standard C main program */
पूर्णांक main(पूर्णांक argc, अक्षर **argv)
{
	output = मानक_निकास;

	खाता* आदान = ख_खोलो(argv[1], "r");
	output = मानक_निकास;
	जर(रिक्त != argv[2])
	{
		output = ख_खोलो(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(आदान);

	/* Set file as executable */
	परत निकास_सफल;
}
