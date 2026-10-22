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
ចំនួនគត់ ip;

unsigned GetTarget(var_na c)
{
	sa_nracanaa entry* i;
	សម្រាប់(i = jump_table; NULL != i; i = i->next)
	{
		ប្រសិនបើ(c == i->name)
		{
			ត្រឡប់ i->target;
		}
	}
	ត្រឡប់ 0;
}

ចំនួនគត់ storeLabel(FILE* source_file, ចំនួនគត់ ip)
{
	sa_nracanaa entry* entry = calloc(1, parimaa_na(sa_nracanaa entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	ត្រឡប់ -1;
}


void outputPointer(ចំនួនគត់ displacement, ចំនួនគត់ number_of_bytes)
{
	unsigned value = displacement;

	ខណៈពេល(number_of_bytes > 0)
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
	ប្រសិនបើ(in_set(ch, "%")) ip = ip + 4;
	បើមិនដូច្នេះ exit(EXIT_FAILURE);
}

void storePointer(var_na ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	ចំនួនគត់ target = GetTarget(ch);
	ចំនួនគត់ displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	ប្រសិនបើ('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	បើមិនដូច្នេះ exit(EXIT_FAILURE);
}

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
void process_byte(var_na c, FILE* source_file, ចំនួនគត់ write)
{
	ប្រសិនបើ(0 <= hex(c, source_file))
	{
		ប្រសិនបើ(toggle)
		{
			ប្រសិនបើ(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
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
		/* Check for and deal with label */
		ប្រសិនបើ(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ប្រសិនបើ(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(input);
		}
		បើមិនដូច្នេះ process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	ចំនួនគត់ c;
	សម្រាប់(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		ប្រសិនបើ(':' == c) c = fgetc(input); /* Deal with : */
		បើមិនដូច្នេះ ប្រសិនបើ(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		បើមិនដូច្នេះ process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
ចំនួនគត់ main(ចំនួនគត់ argc, var_na **argv)
{
	jump_table = NULL;
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ប្រសិនបើ(NULL != argv[2])
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
	ត្រឡប់ EXIT_SUCCESS;
}
