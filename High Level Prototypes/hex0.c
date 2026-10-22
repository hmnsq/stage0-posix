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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <unistd.h>
#شامل <sys/stat.h>
#شامل "../M2libc/bootstrappable.h"

#define max_string 4096

/* Globals */
FILE* output;

تهی سطر_Comment(FILE* source_file)
{
	صحیح c = fgetc(source_file);
	وقتی(!in_set(c, "\n\r"))
	{
		اگر(EOF == c) بشکن;
		c = fgetc(source_file);
	}
}

صحیح hex(صحیح c, FILE* source_file)
{
	اگر (in_set(c, "0123456789")) بازگشت (c - 48);
	وگرنه اگر (in_set(c, "abcdef")) بازگشت (c - 87);
	وگرنه اگر (in_set(c, "ABCDEF")) بازگشت (c - 55);
	وگرنه اگر (in_set(c, "#;")) سطر_Comment(source_file);
	بازگشت -1;
}

صحیح hold;
صحیح toggle;
تهی process_byte(نویسه c, FILE* source_file)
{
	اگر(0 <= hex(c, source_file))
	{
		اگر(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		وگرنه
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

تهی first_pass(FILE* درون_داد)
{
	toggle = FALSE;
	صحیح c;
	برای(c = fgetc(درون_داد); EOF != c; c = fgetc(درون_داد))
	{
		process_byte(c, درون_داد);
	}
}

/* Standard C main program */
صحیح اصلی(صحیح argc, نویسه **argv)
{
	output = stdout;

	FILE* درون_داد = fopen(argv[1], "r");
	output = stdout;
	اگر(پوچ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(درون_داد);

	/* Set file as executable */
	بازگشت EXIT_SUCCESS;
}
