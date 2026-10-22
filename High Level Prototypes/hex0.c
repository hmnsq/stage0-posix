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
	ຈຳນວນເຕັມ c = fgetc(source_file);
	ໃນຂະນະທີ່(!in_set(c, "\n\r"))
	{
		ຖ້າ(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

ຈຳນວນເຕັມ hex(ຈຳນວນເຕັມ c, FILE* source_file)
{
	ຖ້າ (in_set(c, "0123456789")) ສົ່ງຄືນ (c - 48);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_set(c, "abcdef")) ສົ່ງຄືນ (c - 87);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_set(c, "ABCDEF")) ສົ່ງຄືນ (c - 55);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_set(c, "#;")) line_Comment(source_file);
	ສົ່ງຄືນ -1;
}

ຈຳນວນເຕັມ hold;
ຈຳນວນເຕັມ toggle;
void process_byte(var_na c, FILE* source_file)
{
	ຖ້າ(0 <= hex(c, source_file))
	{
		ຖ້າ(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	ຈຳນວນເຕັມ c;
	ສຳລັບ(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc, var_na **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ຖ້າ(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	ສົ່ງຄືນ EXIT_SUCCESS;
}
