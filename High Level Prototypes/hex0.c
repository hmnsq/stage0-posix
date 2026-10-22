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

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ "../M2libc/bootstrappable.h"

#define max_string 4096

/* Globals */
FILE* output;

শূন্য সাৰি_Comment(FILE* source_file)
{
	পূর্ণাঙ্ক c = fgetc(source_file);
	যেতিয়ালৈ(!in_set(c, "\n\r"))
	{
		যদি(EOF == c) ভঙ্গ;
		c = fgetc(source_file);
	}
}

পূর্ণাঙ্ক hex(পূর্ণাঙ্ক c, FILE* source_file)
{
	যদি (in_set(c, "0123456789")) ঘূৰাই (c - 48);
	অন্যথা যদি (in_set(c, "abcdef")) ঘূৰাই (c - 87);
	অন্যথা যদি (in_set(c, "ABCDEF")) ঘূৰাই (c - 55);
	অন্যথা যদি (in_set(c, "#;")) সাৰি_Comment(source_file);
	ঘূৰাই -1;
}

পূর্ণাঙ্ক hold;
পূর্ণাঙ্ক toggle;
শূন্য process_byte(ৱর্ণ c, FILE* source_file)
{
	যদি(0 <= hex(c, source_file))
	{
		যদি(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		অন্যথা
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

শূন্য first_pass(FILE* প্ৰবেশ)
{
	toggle = FALSE;
	পূর্ণাঙ্ক c;
	বাবে(c = fgetc(প্ৰবেশ); EOF != c; c = fgetc(প্ৰবেশ))
	{
		process_byte(c, প্ৰবেশ);
	}
}

/* Standard C main program */
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ **argv)
{
	output = stdout;

	FILE* প্ৰবেশ = fopen(argv[1], "r");
	output = stdout;
	যদি(রিক্ত != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(প্ৰবেশ);

	/* Set file as executable */
	ঘূৰাই EXIT_SUCCESS;
}
