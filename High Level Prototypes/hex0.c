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
	පූර්ණ_සංඛ්‍යා c = fgetc(source_file);
	අතරතුර(!in_set(c, "\n\r"))
	{
		නම්(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

පූර්ණ_සංඛ්‍යා hex(පූර්ණ_සංඛ්‍යා c, FILE* source_file)
{
	නම් (in_set(c, "0123456789")) ආපසු (c - 48);
	නැතහොත් නම් (in_set(c, "abcdef")) ආපසු (c - 87);
	නැතහොත් නම් (in_set(c, "ABCDEF")) ආපසු (c - 55);
	නැතහොත් නම් (in_set(c, "#;")) line_Comment(source_file);
	ආපසු -1;
}

පූර්ණ_සංඛ්‍යා hold;
පූර්ණ_සංඛ්‍යා toggle;
void process_byte(var_na c, FILE* source_file)
{
	නම්(0 <= hex(c, source_file))
	{
		නම්(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		නැතහොත්
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	පූර්ණ_සංඛ්‍යා c;
	සඳහා(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
පූර්ණ_සංඛ්‍යා main(පූර්ණ_සංඛ්‍යා argc, var_na **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	නම්(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	ආපසු EXIT_SUCCESS;
}
