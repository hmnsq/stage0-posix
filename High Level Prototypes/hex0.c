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
FILE* output;

शून्य पङ्क्तिः_Comment(FILE* source_file)
{
	पूर्णाङ्क c = fgetc(source_file);
	जाव(!in_set(c, "\n\r"))
	{
		जइ(EOF == c) भङ्ग;
		c = fgetc(source_file);
	}
}

पूर्णाङ्क hex(पूर्णाङ्क c, FILE* source_file)
{
	जइ (in_set(c, "0123456789")) प्रत्यागम (c - 48);
	अण्णहा जइ (in_set(c, "abcdef")) प्रत्यागम (c - 87);
	अण्णहा जइ (in_set(c, "ABCDEF")) प्रत्यागम (c - 55);
	अण्णहा जइ (in_set(c, "#;")) पङ्क्तिः_Comment(source_file);
	प्रत्यागम -1;
}

पूर्णाङ्क hold;
पूर्णाङ्क toggle;
शून्य process_byte(वर्ण c, FILE* source_file)
{
	जइ(0 <= hex(c, source_file))
	{
		जइ(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		अण्णहा
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

शून्य first_pass(FILE* निवेशः)
{
	toggle = FALSE;
	पूर्णाङ्क c;
	प्रति(c = fgetc(निवेशः); EOF != c; c = fgetc(निवेशः))
	{
		process_byte(c, निवेशः);
	}
}

/* Standard C main program */
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
	output = stdout;

	FILE* निवेशः = fopen(argv[1], "r");
	output = stdout;
	जइ(रिक्त != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(निवेशः);

	/* Set file as executable */
	प्रत्यागम EXIT_SUCCESS;
}
