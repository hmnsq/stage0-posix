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
	char* name;
};


/* Globals */
FILE* output;
struct entry* jump_table;
số_nguyên Base_Address;
số_nguyên ip;
char* scratch;

số_nguyên consume_token(FILE* source_file)
{
	số_nguyên i = 0;
	số_nguyên c = fgetc(source_file);
	trong_khi(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		nếu(EOF == c) break;
	}

	trả_về c;
}

số_nguyên Throwaway_token(FILE* source_file)
{
	số_nguyên c;
	do
	{
		c = fgetc(source_file);
		nếu(EOF == c) break;
	} trong_khi(!in_set(c, " \t\n>"));

	trả_về c;
}

số_nguyên length(char* s)
{
	số_nguyên i = 0;
	trong_khi(0 != s[i]) i = i + 1;
	trả_về i;
}

void Clear_Scratch(char* s)
{
	do
	{
		s[0] = 0;
		s = s + 1;
	} trong_khi(0 != s[0]);
}

void Copy_String(char* a, char* b)
{
	trong_khi(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(char* c)
{
	struct entry* i;
	cho(i = jump_table; NULL != i; i = i->next)
	{
		nếu(match(c, i->name))
		{
			trả_về i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

số_nguyên storeLabel(FILE* source_file, số_nguyên ip)
{
	struct entry* entry = calloc(1, sizeof(struct entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	số_nguyên c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, sizeof(char));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	trả_về c;
}

void outputPointer(số_nguyên displacement, số_nguyên number_of_bytes)
{
	unsigned value = displacement;

	trong_khi(number_of_bytes > 0)
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
	nếu(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	không_thì nếu(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	không_thì nếu('!' == ch) ip = ip + 1; /* Deal with ! */
	không_thì exit(EXIT_FAILURE);
}

void storePointer(char ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	số_nguyên base_sep_p = consume_token(source_file);

	/* Lookup token */
	số_nguyên target = GetTarget(scratch);
	số_nguyên displacement;

	số_nguyên base = ip;

	/* Change relative base address to :<base> */
	nếu ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	không_thì
	{
		displacement = base - target;
	}

	/* output calculated difference */
	nếu('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	không_thì nếu('$' == ch) outputPointer(target, 2); /* Deal with $ */
	không_thì nếu('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	không_thì nếu('&' == ch) outputPointer(target, 4); /* Deal with & */
	không_thì nếu('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	không_thì exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	số_nguyên c = fgetc(source_file);
	trong_khi(!in_set(c, "\n\r"))
	{
		nếu(EOF == c) break;
		c = fgetc(source_file);
	}
}

số_nguyên hex(số_nguyên c, FILE* source_file)
{
	nếu (in_set(c, "0123456789")) trả_về (c - 48);
	không_thì nếu (in_set(c, "abcdef")) trả_về (c - 87);
	không_thì nếu (in_set(c, "ABCDEF")) trả_về (c - 55);
	không_thì nếu (in_set(c, "#;")) line_Comment(source_file);
	trả_về -1;
}

số_nguyên hold;
số_nguyên toggle;
void process_byte(char c, FILE* source_file, số_nguyên write)
{
	nếu(0 <= hex(c, source_file))
	{
		nếu(toggle)
		{
			nếu(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		không_thì
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	số_nguyên c;
	cho(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		nếu(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		nếu(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(input);
			nếu ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(input);
			}
		}
		không_thì process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	số_nguyên c;
	cho(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		nếu(':' == c) c = Throwaway_token(input); /* Deal with : */
		không_thì nếu(in_set(c, "!@$~%&")) storePointer(c, input);  /* Deal with !, @, $, ~, % and & */
		không_thì process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
số_nguyên main(số_nguyên argc, char **argv)
{
	jump_table = NULL;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, sizeof(char));

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	nếu(NULL != argv[2])
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
	trả_về EXIT_SUCCESS;
}
