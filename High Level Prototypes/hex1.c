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
จำนวนเต็ม ip;

unsigned GetTarget(var_na c)
{
	sa_nracanaa entry* i;
	สำหรับ(i = jump_table; NULL != i; i = i->next)
	{
		ถ้า(c == i->name)
		{
			คืน i->target;
		}
	}
	คืน 0;
}

จำนวนเต็ม storeLabel(FILE* source_file, จำนวนเต็ม ip)
{
	sa_nracanaa entry* entry = calloc(1, parimaa_na(sa_nracanaa entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	คืน -1;
}


void outputPointer(จำนวนเต็ม displacement, จำนวนเต็ม number_of_bytes)
{
	unsigned value = displacement;

	ขณะ(number_of_bytes > 0)
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
	ถ้า(in_set(ch, "%")) ip = ip + 4;
	มิฉะนั้น exit(EXIT_FAILURE);
}

void storePointer(var_na ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	จำนวนเต็ม target = GetTarget(ch);
	จำนวนเต็ม displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	ถ้า('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	มิฉะนั้น exit(EXIT_FAILURE);
}

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
void process_byte(var_na c, FILE* source_file, จำนวนเต็ม write)
{
	ถ้า(0 <= hex(c, source_file))
	{
		ถ้า(toggle)
		{
			ถ้า(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
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
		/* Check for and deal with label */
		ถ้า(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ถ้า(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(input);
		}
		มิฉะนั้น process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	จำนวนเต็ม c;
	สำหรับ(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		ถ้า(':' == c) c = fgetc(input); /* Deal with : */
		มิฉะนั้น ถ้า(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		มิฉะนั้น process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
จำนวนเต็ม หลัก(จำนวนเต็ม argc, var_na **argv)
{
	jump_table = NULL;
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ถ้า(NULL != argv[2])
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
	คืน EXIT_SUCCESS;
}
