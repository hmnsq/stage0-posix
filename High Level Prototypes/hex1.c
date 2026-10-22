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

struct entry
{
	struct entry* next;
	unsigned target;
	char name;
};


/* Globals */
FILE* output;
struct entry* jump_table;
ціле ip;

unsigned GetTarget(char c)
{
	struct entry* i;
	для(i = jump_table; NULL != i; i = i->next)
	{
		якщо(c == i->name)
		{
			повернути i->target;
		}
	}
	повернути 0;
}

ціле storeLabel(FILE* source_file, ціле ip)
{
	struct entry* entry = calloc(1, sizeof(struct entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	повернути -1;
}


void outputPointer(ціле displacement, ціле number_of_bytes)
{
	unsigned value = displacement;

	поки(number_of_bytes > 0)
	{
		unsigned byte = value % 256;
		value = value / 256;
		fputc(byte, output);
		number_of_bytes = number_of_bytes - 1;
	}
}

void Update_Pointer(char ch)
{
	/* Calculate pointer size*/
	якщо(in_set(ch, "%")) ip = ip + 4;
	інакше exit(EXIT_FAILURE);
}

void storePointer(char ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	ціле target = GetTarget(ch);
	ціле displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	якщо('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	інакше exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	ціле c = fgetc(source_file);
	поки(!in_set(c, "\n\r"))
	{
		якщо(EOF == c) break;
		c = fgetc(source_file);
	}
}

ціле hex(ціле c, FILE* source_file)
{
	якщо (in_set(c, "0123456789")) повернути (c - 48);
	інакше якщо (in_set(c, "abcdef")) повернути (c - 87);
	інакше якщо (in_set(c, "ABCDEF")) повернути (c - 55);
	інакше якщо (in_set(c, "#;")) line_Comment(source_file);
	повернути -1;
}

ціле hold;
ціле toggle;
void process_byte(char c, FILE* source_file, ціле write)
{
	якщо(0 <= hex(c, source_file))
	{
		якщо(toggle)
		{
			якщо(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		інакше
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	ціле c;
	для(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		якщо(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		якщо(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(input);
		}
		інакше process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	ціле c;
	для(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		якщо(':' == c) c = fgetc(input); /* Deal with : */
		інакше якщо(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		інакше process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
ціле main(ціле argc, char **argv)
{
	jump_table = NULL;
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	якщо(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(input);
	rewind(input);

	/* Fix all the references*/
	ip = 0;
	second_pass(input);

	/* Set file as executable */
	повернути EXIT_SUCCESS;
}
