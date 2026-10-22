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
	ចំនួនគត់ c = fgetc(source_file);
	ខណៈពេល(!in_set(c, "\n\r"))
	{
		ប្រសិនបើ(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

ចំនួនគត់ hex(ចំនួនគត់ c, FILE* source_file)
{
	ប្រសិនបើ (in_set(c, "0123456789")) ត្រឡប់ (c - 48);
	បើមិនដូច្នេះ ប្រសិនបើ (in_set(c, "abcdef")) ត្រឡប់ (c - 87);
	បើមិនដូច្នេះ ប្រសិនបើ (in_set(c, "ABCDEF")) ត្រឡប់ (c - 55);
	បើមិនដូច្នេះ ប្រសិនបើ (in_set(c, "#;")) line_Comment(source_file);
	ត្រឡប់ -1;
}

ចំនួនគត់ hold;
ចំនួនគត់ toggle;
void process_byte(var_na c, FILE* source_file)
{
	ប្រសិនបើ(0 <= hex(c, source_file))
	{
		ប្រសិនបើ(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		បើមិនដូច្នេះ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	ចំនួនគត់ c;
	សម្រាប់(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
ចំនួនគត់ main(ចំនួនគត់ argc, var_na **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ប្រសិនបើ(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	ត្រឡប់ EXIT_SUCCESS;
}
