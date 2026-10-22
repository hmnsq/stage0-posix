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
ကိန်းပြည့် Base_Address;
ကိန်းပြည့် ip;
var_na* scratch;

ကိန်းပြည့် consume_token(FILE* source_file)
{
	ကိန်းပြည့် i = 0;
	ကိန်းပြည့် c = fgetc(source_file);
	နေစဉ်(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		အကယ်၍(EOF == c) bha_nga;
	}

	ပြန်ပေး c;
}

ကိန်းပြည့် Throwaway_token(FILE* source_file)
{
	ကိန်းပြည့် c;
	do
	{
		c = fgetc(source_file);
		အကယ်၍(EOF == c) bha_nga;
	} နေစဉ်(!in_set(c, " \t\n>"));

	ပြန်ပေး c;
}

ကိန်းပြည့် length(var_na* s)
{
	ကိန်းပြည့် i = 0;
	နေစဉ်(0 != s[i]) i = i + 1;
	ပြန်ပေး i;
}

void Clear_Scratch(var_na* s)
{
	do
	{
		s[0] = 0;
		s = s + 1;
	} နေစဉ်(0 != s[0]);
}

void Copy_String(var_na* a, var_na* b)
{
	နေစဉ်(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(var_na* c)
{
	sa_nracanaa entry* i;
	အတွက်(i = jump_table; NULL != i; i = i->next)
	{
		အကယ်၍(match(c, i->name))
		{
			ပြန်ပေး i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

ကိန်းပြည့် storeLabel(FILE* source_file, ကိန်းပြည့် ip)
{
	sa_nracanaa entry* entry = calloc(1, parimaa_na(sa_nracanaa entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	ကိန်းပြည့် c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, parimaa_na(var_na));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ပြန်ပေး c;
}

void outputPointer(ကိန်းပြည့် displacement, ကိန်းပြည့် number_of_bytes)
{
	unsigned value = displacement;

	နေစဉ်(number_of_bytes > 0)
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
	အကယ်၍(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	မဟုတ်ပါက အကယ်၍(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	မဟုတ်ပါက အကယ်၍('!' == ch) ip = ip + 1; /* Deal with ! */
	မဟုတ်ပါက exit(EXIT_FAILURE);
}

void storePointer(var_na ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	ကိန်းပြည့် base_sep_p = consume_token(source_file);

	/* Lookup token */
	ကိန်းပြည့် target = GetTarget(scratch);
	ကိန်းပြည့် displacement;

	ကိန်းပြည့် base = ip;

	/* Change relative base address to :<base> */
	အကယ်၍ ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	မဟုတ်ပါက
	{
		displacement = base - target;
	}

	/* output calculated difference */
	အကယ်၍('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	မဟုတ်ပါက အကယ်၍('$' == ch) outputPointer(target, 2); /* Deal with $ */
	မဟုတ်ပါက အကယ်၍('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	မဟုတ်ပါက အကယ်၍('&' == ch) outputPointer(target, 4); /* Deal with & */
	မဟုတ်ပါက အကယ်၍('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	မဟုတ်ပါက exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	ကိန်းပြည့် c = fgetc(source_file);
	နေစဉ်(!in_set(c, "\n\r"))
	{
		အကယ်၍(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

ကိန်းပြည့် hex(ကိန်းပြည့် c, FILE* source_file)
{
	အကယ်၍ (in_set(c, "0123456789")) ပြန်ပေး (c - 48);
	မဟုတ်ပါက အကယ်၍ (in_set(c, "abcdef")) ပြန်ပေး (c - 87);
	မဟုတ်ပါက အကယ်၍ (in_set(c, "ABCDEF")) ပြန်ပေး (c - 55);
	မဟုတ်ပါက အကယ်၍ (in_set(c, "#;")) line_Comment(source_file);
	ပြန်ပေး -1;
}

ကိန်းပြည့် hold;
ကိန်းပြည့် toggle;
void process_byte(var_na c, FILE* source_file, ကိန်းပြည့် write)
{
	အကယ်၍(0 <= hex(c, source_file))
	{
		အကယ်၍(toggle)
		{
			အကယ်၍(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		မဟုတ်ပါက
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	ကိန်းပြည့် c;
	အတွက်(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		အကယ်၍(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		အကယ်၍(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(input);
			အကယ်၍ ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(input);
			}
		}
		မဟုတ်ပါက process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	ကိန်းပြည့် c;
	အတွက်(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		အကယ်၍(':' == c) c = Throwaway_token(input); /* Deal with : */
		မဟုတ်ပါက အကယ်၍(in_set(c, "!@$~%&")) storePointer(c, input);  /* Deal with !, @, $, ~, % and & */
		မဟုတ်ပါက process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
ကိန်းပြည့် main(ကိန်းပြည့် argc, var_na **argv)
{
	jump_table = NULL;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, parimaa_na(var_na));

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	အကယ်၍(NULL != argv[2])
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
	ပြန်ပေး EXIT_SUCCESS;
}
