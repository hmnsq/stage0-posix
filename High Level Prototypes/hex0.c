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
	จำนวนเต็ม c = fgetc(source_file);
	ขณะ(!in_set(c, "\n\r"))
	{
		ถ้า(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

จำนวนเต็ม hex(จำนวนเต็ม c, FILE* source_file)
{
	ถ้า (in_set(c, "0123456789")) คืน (c - 48);
	มิฉะนั้น ถ้า (in_set(c, "abcdef")) คืน (c - 87);
	มิฉะนั้น ถ้า (in_set(c, "ABCDEF")) คืน (c - 55);
	มิฉะนั้น ถ้า (in_set(c, "#;")) line_Comment(source_file);
	คืน -1;
}

จำนวนเต็ม hold;
จำนวนเต็ม toggle;
void process_byte(var_na c, FILE* source_file)
{
	ถ้า(0 <= hex(c, source_file))
	{
		ถ้า(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		มิฉะนั้น
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	จำนวนเต็ม c;
	สำหรับ(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
จำนวนเต็ม หลัก(จำนวนเต็ม argc, var_na **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ถ้า(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	คืน EXIT_SUCCESS;
}
