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
цео Base_Address;
цео ip;
char* scratch;

цео consume_token(FILE* source_file)
{
	цео i = 0;
	цео c = fgetc(source_file);
	док(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		ако(EOF == c) break;
	}

	врати c;
}

цео Throwaway_token(FILE* source_file)
{
	цео c;
	do
	{
		c = fgetc(source_file);
		ако(EOF == c) break;
	} док(!in_set(c, " \t\n>"));

	врати c;
}

цео length(char* s)
{
	цео i = 0;
	док(0 != s[i]) i = i + 1;
	врати i;
}

void Clear_Scratch(char* s)
{
	do
	{
		s[0] = 0;
		s = s + 1;
	} док(0 != s[0]);
}

void Copy_String(char* a, char* b)
{
	док(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(char* c)
{
	struct entry* i;
	за(i = jump_table; NULL != i; i = i->next)
	{
		ако(match(c, i->name))
		{
			врати i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

цео storeLabel(FILE* source_file, цео ip)
{
	struct entry* entry = calloc(1, sizeof(struct entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	цео c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, sizeof(char));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	врати c;
}

void outputPointer(цео displacement, цео number_of_bytes)
{
	unsigned value = displacement;

	док(number_of_bytes > 0)
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
	ако(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	инако ако(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	инако ако('!' == ch) ip = ip + 1; /* Deal with ! */
	инако exit(EXIT_FAILURE);
}

void storePointer(char ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	цео base_sep_p = consume_token(source_file);

	/* Lookup token */
	цео target = GetTarget(scratch);
	цео displacement;

	цео base = ip;

	/* Change relative base address to :<base> */
	ако ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	инако
	{
		displacement = base - target;
	}

	/* output calculated difference */
	ако('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	инако ако('$' == ch) outputPointer(target, 2); /* Deal with $ */
	инако ако('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	инако ако('&' == ch) outputPointer(target, 4); /* Deal with & */
	инако ако('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	инако exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	цео c = fgetc(source_file);
	док(!in_set(c, "\n\r"))
	{
		ако(EOF == c) break;
		c = fgetc(source_file);
	}
}

цео hex(цео c, FILE* source_file)
{
	ако (in_set(c, "0123456789")) врати (c - 48);
	инако ако (in_set(c, "abcdef")) врати (c - 87);
	инако ако (in_set(c, "ABCDEF")) врати (c - 55);
	инако ако (in_set(c, "#;")) line_Comment(source_file);
	врати -1;
}

цео hold;
цео toggle;
void process_byte(char c, FILE* source_file, цео write)
{
	ако(0 <= hex(c, source_file))
	{
		ако(toggle)
		{
			ако(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		инако
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	цео c;
	за(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		ако(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ако(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(input);
			ако ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(input);
			}
		}
		инако process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	цео c;
	за(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		ако(':' == c) c = Throwaway_token(input); /* Deal with : */
		инако ако(in_set(c, "!@$~%&")) storePointer(c, input);  /* Deal with !, @, $, ~, % and & */
		инако process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
цео main(цео argc, char **argv)
{
	jump_table = NULL;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, sizeof(char));

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ако(NULL != argv[2])
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
	врати EXIT_SUCCESS;
}
