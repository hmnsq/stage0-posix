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

sa_nracanaa entry
{
	sa_nracanaa entry* next;
	unsigned target;
	var_na name;
};


/* Globals */
FILE* output;
sa_nracanaa entry* jump_table;
ཧྲིལ ip;

unsigned GetTarget(var_na c)
{
	sa_nracanaa entry* i;
	དོན་ལུ(i = jump_table; NULL != i; i = i->next)
	{
		གལ་སྲིད(c == i->name)
		{
			ལོག i->target;
		}
	}
	ལོག 0;
}

ཧྲིལ storeLabel(FILE* source_file, ཧྲིལ ip)
{
	sa_nracanaa entry* entry = calloc(1, parimaa_na(sa_nracanaa entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	ལོག -1;
}


void outputPointer(ཧྲིལ displacement, ཧྲིལ number_of_bytes)
{
	unsigned value = displacement;

	སྐབས(number_of_bytes > 0)
	{
		unsigned byte = value % 256;
		value = value / 256;
		fputc(byte, output);
		number_of_bytes = number_of_bytes - 1;
	}
}

void Update_Pointer(var_na ch)
{
	/* Calculate pointer size*/
	གལ་སྲིད(in_set(ch, "%")) ip = ip + 4;
	དེ་མིན exit(EXIT_FAILURE);
}

void storePointer(var_na ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	ཧྲིལ target = GetTarget(ch);
	ཧྲིལ displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	གལ་སྲིད('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	དེ་མིན exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	ཧྲིལ c = fgetc(source_file);
	སྐབས(!in_set(c, "\n\r"))
	{
		གལ་སྲིད(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

ཧྲིལ hex(ཧྲིལ c, FILE* source_file)
{
	གལ་སྲིད (in_set(c, "0123456789")) ལོག (c - 48);
	དེ་མིན གལ་སྲིད (in_set(c, "abcdef")) ལོག (c - 87);
	དེ་མིན གལ་སྲིད (in_set(c, "ABCDEF")) ལོག (c - 55);
	དེ་མིན གལ་སྲིད (in_set(c, "#;")) line_Comment(source_file);
	ལོག -1;
}

ཧྲིལ hold;
ཧྲིལ toggle;
void process_byte(var_na c, FILE* source_file, ཧྲིལ write)
{
	གལ་སྲིད(0 <= hex(c, source_file))
	{
		གལ་སྲིད(toggle)
		{
			གལ་སྲིད(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		དེ་མིན
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	ཧྲིལ c;
	དོན་ལུ(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		གལ་སྲིད(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		གལ་སྲིད(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(input);
		}
		དེ་མིན process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	ཧྲིལ c;
	དོན་ལུ(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		གལ་སྲིད(':' == c) c = fgetc(input); /* Deal with : */
		དེ་མིན གལ་སྲིད(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		དེ་མིན process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
ཧྲིལ main(ཧྲིལ argc, var_na **argv)
{
	jump_table = NULL;
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	གལ་སྲིད(NULL != argv[2])
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
	ལོག EXIT_SUCCESS;
}
