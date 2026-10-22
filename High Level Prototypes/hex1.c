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
puur_naa_nka ip;

unsigned GetTarget(var_na c)
{
	sa_nracanaa entry* i;
	for(i = jump_table; NULL != i; i = i->next)
	{
		ᮚᮦᮔ᮪(c == i->name)
		{
			ᮝᮜᮤ i->target;
		}
	}
	ᮝᮜᮤ 0;
}

puur_naa_nka storeLabel(FILE* source_file, puur_naa_nka ip)
{
	sa_nracanaa entry* entry = calloc(1, parimaa_na(sa_nracanaa entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	ᮝᮜᮤ -1;
}


void outputPointer(puur_naa_nka displacement, puur_naa_nka number_of_bytes)
{
	unsigned value = displacement;

	ᮞᮨᮓᮨᮜ(number_of_bytes > 0)
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
	ᮚᮦᮔ᮪(in_set(ch, "%")) ip = ip + 4;
	ᮜᮤᮚ exit(EXIT_FAILURE);
}

void storePointer(var_na ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	puur_naa_nka target = GetTarget(ch);
	puur_naa_nka displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	ᮚᮦᮔ᮪('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	ᮜᮤᮚ exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	puur_naa_nka c = fgetc(source_file);
	ᮞᮨᮓᮨᮜ(!in_set(c, "\n\r"))
	{
		ᮚᮦᮔ᮪(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

puur_naa_nka hex(puur_naa_nka c, FILE* source_file)
{
	ᮚᮦᮔ᮪ (in_set(c, "0123456789")) ᮝᮜᮤ (c - 48);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (in_set(c, "abcdef")) ᮝᮜᮤ (c - 87);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (in_set(c, "ABCDEF")) ᮝᮜᮤ (c - 55);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (in_set(c, "#;")) line_Comment(source_file);
	ᮝᮜᮤ -1;
}

puur_naa_nka hold;
puur_naa_nka toggle;
void process_byte(var_na c, FILE* source_file, puur_naa_nka write)
{
	ᮚᮦᮔ᮪(0 <= hex(c, source_file))
	{
		ᮚᮦᮔ᮪(toggle)
		{
			ᮚᮦᮔ᮪(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ᮜᮤᮚ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	puur_naa_nka c;
	for(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		ᮚᮦᮔ᮪(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ᮚᮦᮔ᮪(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(input);
		}
		ᮜᮤᮚ process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	puur_naa_nka c;
	for(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		ᮚᮦᮔ᮪(':' == c) c = fgetc(input); /* Deal with : */
		ᮜᮤᮚ ᮚᮦᮔ᮪(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		ᮜᮤᮚ process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
puur_naa_nka main(puur_naa_nka argc, var_na **argv)
{
	jump_table = NULL;
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ᮚᮦᮔ᮪(NULL != argv[2])
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
	ᮝᮜᮤ EXIT_SUCCESS;
}
