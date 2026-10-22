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

शून्य पङ्क्ति_Comment(खाता* source_file)
{
	पूर्णाङ्क c = ख_अक्षर_लो(source_file);
	जबसम्म(!in_set(c, "\n\r"))
	{
		यदि(खातापूर्ण == c) रोक;
		c = ख_अक्षर_लो(source_file);
	}
}

पूर्णाङ्क hex(पूर्णाङ्क c, खाता* source_file)
{
	यदि (in_set(c, "0123456789")) फर्काऊ (c - 48);
	अन्यथा यदि (in_set(c, "abcdef")) फर्काऊ (c - 87);
	अन्यथा यदि (in_set(c, "ABCDEF")) फर्काऊ (c - 55);
	अन्यथा यदि (in_set(c, "#;")) पङ्क्ति_Comment(source_file);
	फर्काऊ -1;
}

पूर्णाङ्क hold;
पूर्णाङ्क toggle;
शून्य process_byte(वर्ण c, खाता* source_file)
{
	यदि(0 <= hex(c, source_file))
	{
		यदि(toggle)
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

शून्य first_pass(खाता* आगत)
{
	toggle = FALSE;
	पूर्णाङ्क c;
	लागि(c = ख_अक्षर_लो(आगत); खातापूर्ण != c; c = ख_अक्षर_लो(आगत))
	{
		process_byte(c, आगत);
	}
}

/* Standard C main program */
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
	output = मानक_निकास;

	खाता* आगत = ख_खोलो(argv[1], "r");
	output = मानक_निकास;
	यदि(रिक्त != argv[2])
	{
		output = ख_खोलो(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(आगत);

	/* Set file as executable */
	फर्काऊ निकास_सफल;
}
