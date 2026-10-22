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
	정수 c = fgetc(source_file);
	동안(!in_set(c, "\n\r"))
	{
		만약(EOF == c) break;
		c = fgetc(source_file);
	}
}

정수 hex(정수 c, FILE* source_file)
{
	만약 (in_set(c, "0123456789")) 반환 (c - 48);
	아니면 만약 (in_set(c, "abcdef")) 반환 (c - 87);
	아니면 만약 (in_set(c, "ABCDEF")) 반환 (c - 55);
	아니면 만약 (in_set(c, "#;")) line_Comment(source_file);
	반환 -1;
}

정수 hold;
정수 toggle;
void process_byte(char c, FILE* source_file)
{
	만약(0 <= hex(c, source_file))
	{
		만약(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		아니면
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	정수 c;
	위해(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
정수 main(정수 argc, char **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	만약(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	반환 EXIT_SUCCESS;
}
