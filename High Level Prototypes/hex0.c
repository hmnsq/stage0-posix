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
	ကိန်းပြည့် c = fgetc(source_file);
	နေစဉ်(!in_set(c, "\n\r"))
	{
		အကယ်၍(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

ကိန်းပြည့် hex(ကိန်းပြည့် c, FILE* source_file)
{
	အကယ်၍ (in_set(c, "0123456789")) ပြန်ပေး (c - 48);
	မဟုတ်ပါက အကယ်၍ (in_set(c, "abcdef")) ပြန်ပေး (c - 87);
	မဟုတ်ပါက အကယ်၍ (in_set(c, "ABCDEF")) ပြန်ပေး (c - 55);
	မဟုတ်ပါက အကယ်၍ (in_set(c, "#;")) line_Comment(source_file);
	ပြန်ပေး -1;
}

ကိန်းပြည့် hold;
ကိန်းပြည့် toggle;
void process_byte(var_na c, FILE* source_file)
{
	အကယ်၍(0 <= hex(c, source_file))
	{
		အကယ်၍(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		မဟုတ်ပါက
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	ကိန်းပြည့် c;
	အတွက်(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
ကိန်းပြည့် main(ကိန်းပြည့် argc, var_na **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	အကယ်၍(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	ပြန်ပေး EXIT_SUCCESS;
}
