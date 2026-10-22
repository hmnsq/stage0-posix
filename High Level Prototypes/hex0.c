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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../M2libc/bootstrappable.h"

#define max_string 4096

/* Globals */
FILE* output;

void line_Comment(FILE* source_file)
{
	số_nguyên c = fgetc(source_file);
	trong_khi(!in_set(c, "\n\r"))
	{
		nếu(EOF == c) break;
		c = fgetc(source_file);
	}
}

số_nguyên hex(số_nguyên c, FILE* source_file)
{
	nếu (in_set(c, "0123456789")) trả_về (c - 48);
	không_thì nếu (in_set(c, "abcdef")) trả_về (c - 87);
	không_thì nếu (in_set(c, "ABCDEF")) trả_về (c - 55);
	không_thì nếu (in_set(c, "#;")) line_Comment(source_file);
	trả_về -1;
}

số_nguyên hold;
số_nguyên toggle;
void process_byte(char c, FILE* source_file)
{
	nếu(0 <= hex(c, source_file))
	{
		nếu(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		không_thì
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	số_nguyên c;
	cho(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
số_nguyên main(số_nguyên argc, char **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	nếu(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	trả_về EXIT_SUCCESS;
}
