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
정수 Base_Address;
정수 ip;
char* scratch;

정수 consume_token(FILE* source_file)
{
	정수 i = 0;
	정수 c = fgetc(source_file);
	동안(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		만약(EOF == c) break;
	}

	반환 c;
}

정수 Throwaway_token(FILE* source_file)
{
	정수 c;
	do
	{
		c = fgetc(source_file);
		만약(EOF == c) break;
	} 동안(!in_set(c, " \t\n>"));

	반환 c;
}

정수 length(char* s)
{
	정수 i = 0;
	동안(0 != s[i]) i = i + 1;
	반환 i;
}

void Clear_Scratch(char* s)
{
	do
	{
		s[0] = 0;
		s = s + 1;
	} 동안(0 != s[0]);
}

void Copy_String(char* a, char* b)
{
	동안(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(char* c)
{
	struct entry* i;
	위해(i = jump_table; NULL != i; i = i->next)
	{
		만약(match(c, i->name))
		{
			반환 i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

정수 storeLabel(FILE* source_file, 정수 ip)
{
	struct entry* entry = calloc(1, sizeof(struct entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	정수 c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, sizeof(char));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	반환 c;
}

void outputPointer(정수 displacement, 정수 number_of_bytes)
{
	unsigned value = displacement;

	동안(number_of_bytes > 0)
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
	만약(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	아니면 만약(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	아니면 만약('!' == ch) ip = ip + 1; /* Deal with ! */
	아니면 exit(EXIT_FAILURE);
}

void storePointer(char ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	정수 base_sep_p = consume_token(source_file);

	/* Lookup token */
	정수 target = GetTarget(scratch);
	정수 displacement;

	정수 base = ip;

	/* Change relative base address to :<base> */
	만약 ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	아니면
	{
		displacement = base - target;
	}

	/* output calculated difference */
	만약('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	아니면 만약('$' == ch) outputPointer(target, 2); /* Deal with $ */
	아니면 만약('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	아니면 만약('&' == ch) outputPointer(target, 4); /* Deal with & */
	아니면 만약('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	아니면 exit(EXIT_FAILURE);
}

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
void process_byte(char c, FILE* source_file, 정수 write)
{
	만약(0 <= hex(c, source_file))
	{
		만약(toggle)
		{
			만약(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
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
		/* Check for and deal with label */
		만약(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		만약(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(input);
			만약 ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(input);
			}
		}
		아니면 process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	정수 c;
	위해(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		만약(':' == c) c = Throwaway_token(input); /* Deal with : */
		아니면 만약(in_set(c, "!@$~%&")) storePointer(c, input);  /* Deal with !, @, $, ~, % and & */
		아니면 process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
정수 main(정수 argc, char **argv)
{
	jump_table = NULL;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, sizeof(char));

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	만약(NULL != argv[2])
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
	반환 EXIT_SUCCESS;
}
