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
	var_na* name;
};


/* Globals */
FILE* output;
sa_nracanaa entry* jump_table;
puur_naa_nka Base_Address;
puur_naa_nka ip;
var_na* scratch;

puur_naa_nka consume_token(FILE* source_file)
{
	puur_naa_nka i = 0;
	puur_naa_nka c = fgetc(source_file);
	ᤀᤠ᤺(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		ᤀᤡ᤺(EOF == c) bha_nga;
	}

	ᤀᤡᤴ c;
}

puur_naa_nka Throwaway_token(FILE* source_file)
{
	puur_naa_nka c;
	do
	{
		c = fgetc(source_file);
		ᤀᤡ᤺(EOF == c) bha_nga;
	} ᤀᤠ᤺(!in_set(c, " \t\n>"));

	ᤀᤡᤴ c;
}

puur_naa_nka length(var_na* s)
{
	puur_naa_nka i = 0;
	ᤀᤠ᤺(0 != s[i]) i = i + 1;
	ᤀᤡᤴ i;
}

void Clear_Scratch(var_na* s)
{
	do
	{
		s[0] = 0;
		s = s + 1;
	} ᤀᤠ᤺(0 != s[0]);
}

void Copy_String(var_na* a, var_na* b)
{
	ᤀᤠ᤺(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(var_na* c)
{
	sa_nracanaa entry* i;
	for(i = jump_table; NULL != i; i = i->next)
	{
		ᤀᤡ᤺(match(c, i->name))
		{
			ᤀᤡᤴ i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
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
	puur_naa_nka c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, parimaa_na(var_na));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ᤀᤡᤴ c;
}

void outputPointer(puur_naa_nka displacement, puur_naa_nka number_of_bytes)
{
	unsigned value = displacement;

	ᤀᤠ᤺(number_of_bytes > 0)
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
	ᤀᤡ᤺(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺('!' == ch) ip = ip + 1; /* Deal with ! */
	ᤀᤡ᤺ᤆᤡ exit(EXIT_FAILURE);
}

void storePointer(var_na ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	puur_naa_nka base_sep_p = consume_token(source_file);

	/* Lookup token */
	puur_naa_nka target = GetTarget(scratch);
	puur_naa_nka displacement;

	puur_naa_nka base = ip;

	/* Change relative base address to :<base> */
	ᤀᤡ᤺ ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	ᤀᤡ᤺ᤆᤡ
	{
		displacement = base - target;
	}

	/* output calculated difference */
	ᤀᤡ᤺('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺('$' == ch) outputPointer(target, 2); /* Deal with $ */
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺('&' == ch) outputPointer(target, 4); /* Deal with & */
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	ᤀᤡ᤺ᤆᤡ exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	puur_naa_nka c = fgetc(source_file);
	ᤀᤠ᤺(!in_set(c, "\n\r"))
	{
		ᤀᤡ᤺(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

puur_naa_nka hex(puur_naa_nka c, FILE* source_file)
{
	ᤀᤡ᤺ (in_set(c, "0123456789")) ᤀᤡᤴ (c - 48);
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (in_set(c, "abcdef")) ᤀᤡᤴ (c - 87);
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (in_set(c, "ABCDEF")) ᤀᤡᤴ (c - 55);
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (in_set(c, "#;")) line_Comment(source_file);
	ᤀᤡᤴ -1;
}

puur_naa_nka hold;
puur_naa_nka toggle;
void process_byte(var_na c, FILE* source_file, puur_naa_nka write)
{
	ᤀᤡ᤺(0 <= hex(c, source_file))
	{
		ᤀᤡ᤺(toggle)
		{
			ᤀᤡ᤺(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ᤀᤡ᤺ᤆᤡ
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
		ᤀᤡ᤺(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ᤀᤡ᤺(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(input);
			ᤀᤡ᤺ ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(input);
			}
		}
		ᤀᤡ᤺ᤆᤡ process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	puur_naa_nka c;
	for(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		ᤀᤡ᤺(':' == c) c = Throwaway_token(input); /* Deal with : */
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(in_set(c, "!@$~%&")) storePointer(c, input);  /* Deal with !, @, $, ~, % and & */
		ᤀᤡ᤺ᤆᤡ process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
puur_naa_nka main(puur_naa_nka argc, var_na **argv)
{
	jump_table = NULL;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, parimaa_na(var_na));

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ᤀᤡ᤺(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(input);
	rewind(input);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(input);

	/* Set file as executable */
	ᤀᤡᤴ EXIT_SUCCESS;
}
