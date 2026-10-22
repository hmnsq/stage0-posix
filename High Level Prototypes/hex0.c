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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব "../M2libc/bootstrappable.h"

#define max_string 4096

/* Globals */
FILE* output;

শূন্য সারি_Comment(FILE* source_file)
{
	পূর্ণসংখ্যা c = fgetc(source_file);
	যখন(!in_set(c, "\n\r"))
	{
		যদি(EOF == c) বিরতি;
		c = fgetc(source_file);
	}
}

পূর্ণসংখ্যা hex(পূর্ণসংখ্যা c, FILE* source_file)
{
	যদি (in_set(c, "0123456789")) ফেরত (c - 48);
	অন্যথা যদি (in_set(c, "abcdef")) ফেরত (c - 87);
	অন্যথা যদি (in_set(c, "ABCDEF")) ফেরত (c - 55);
	অন্যথা যদি (in_set(c, "#;")) সারি_Comment(source_file);
	ফেরত -1;
}

পূর্ণসংখ্যা hold;
পূর্ণসংখ্যা toggle;
শূন্য process_byte(অক্ষর c, FILE* source_file)
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

শূন্য first_pass(FILE* প্রবেশ)
{
	toggle = FALSE;
	পূর্ণসংখ্যা c;
	জন্য(c = fgetc(প্রবেশ); EOF != c; c = fgetc(প্রবেশ))
	{
		process_byte(c, প্রবেশ);
	}
}

/* Standard C main program */
পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
	output = stdout;

	FILE* প্রবেশ = fopen(argv[1], "r");
	output = stdout;
	যদি(ফাঁকা != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(প্রবেশ);

	/* Set file as executable */
	ফেরত EXIT_SUCCESS;
}
