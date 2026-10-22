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
පූර්ණ_සංඛ්‍යා Base_Address;
පූර්ණ_සංඛ්‍යා ip;
var_na* scratch;

පූර්ණ_සංඛ්‍යා consume_token(FILE* source_file)
{
	පූර්ණ_සංඛ්‍යා i = 0;
	පූර්ණ_සංඛ්‍යා c = fgetc(source_file);
	අතරතුර(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		නම්(EOF == c) bha_nga;
	}

	ආපසු c;
}

පූර්ණ_සංඛ්‍යා Throwaway_token(FILE* source_file)
{
	පූර්ණ_සංඛ්‍යා c;
	do
	{
		c = fgetc(source_file);
		නම්(EOF == c) bha_nga;
	} අතරතුර(!in_set(c, " \t\n>"));

	ආපසු c;
}

පූර්ණ_සංඛ්‍යා length(var_na* s)
{
	පූර්ණ_සංඛ්‍යා i = 0;
	අතරතුර(0 != s[i]) i = i + 1;
	ආපසු i;
}

void Clear_Scratch(var_na* s)
{
	do
	{
		s[0] = 0;
		s = s + 1;
	} අතරතුර(0 != s[0]);
}

void Copy_String(var_na* a, var_na* b)
{
	අතරතුර(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(var_na* c)
{
	sa_nracanaa entry* i;
	සඳහා(i = jump_table; NULL != i; i = i->next)
	{
		නම්(match(c, i->name))
		{
			ආපසු i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

පූර්ණ_සංඛ්‍යා storeLabel(FILE* source_file, පූර්ණ_සංඛ්‍යා ip)
{
	sa_nracanaa entry* entry = calloc(1, parimaa_na(sa_nracanaa entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	පූර්ණ_සංඛ්‍යා c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, parimaa_na(var_na));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ආපසු c;
}

void outputPointer(පූර්ණ_සංඛ්‍යා displacement, පූර්ණ_සංඛ්‍යා number_of_bytes)
{
	unsigned value = displacement;

	අතරතුර(number_of_bytes > 0)
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
	නම්(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	නැතහොත් නම්(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	නැතහොත් නම්('!' == ch) ip = ip + 1; /* Deal with ! */
	නැතහොත් exit(EXIT_FAILURE);
}

void storePointer(var_na ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	පූර්ණ_සංඛ්‍යා base_sep_p = consume_token(source_file);

	/* Lookup token */
	පූර්ණ_සංඛ්‍යා target = GetTarget(scratch);
	පූර්ණ_සංඛ්‍යා displacement;

	පූර්ණ_සංඛ්‍යා base = ip;

	/* Change relative base address to :<base> */
	නම් ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	නැතහොත්
	{
		displacement = base - target;
	}

	/* output calculated difference */
	නම්('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	නැතහොත් නම්('$' == ch) outputPointer(target, 2); /* Deal with $ */
	නැතහොත් නම්('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	නැතහොත් නම්('&' == ch) outputPointer(target, 4); /* Deal with & */
	නැතහොත් නම්('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	නැතහොත් exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	පූර්ණ_සංඛ්‍යා c = fgetc(source_file);
	අතරතුර(!in_set(c, "\n\r"))
	{
		නම්(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

පූර්ණ_සංඛ්‍යා hex(පූර්ණ_සංඛ්‍යා c, FILE* source_file)
{
	නම් (in_set(c, "0123456789")) ආපසු (c - 48);
	නැතහොත් නම් (in_set(c, "abcdef")) ආපසු (c - 87);
	නැතහොත් නම් (in_set(c, "ABCDEF")) ආපසු (c - 55);
	නැතහොත් නම් (in_set(c, "#;")) line_Comment(source_file);
	ආපසු -1;
}

පූර්ණ_සංඛ්‍යා hold;
පූර්ණ_සංඛ්‍යා toggle;
void process_byte(var_na c, FILE* source_file, පූර්ණ_සංඛ්‍යා write)
{
	නම්(0 <= hex(c, source_file))
	{
		නම්(toggle)
		{
			නම්(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		නැතහොත්
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	පූර්ණ_සංඛ්‍යා c;
	සඳහා(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		නම්(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		නම්(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(input);
			නම් ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(input);
			}
		}
		නැතහොත් process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	පූර්ණ_සංඛ්‍යා c;
	සඳහා(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		නම්(':' == c) c = Throwaway_token(input); /* Deal with : */
		නැතහොත් නම්(in_set(c, "!@$~%&")) storePointer(c, input);  /* Deal with !, @, $, ~, % and & */
		නැතහොත් process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
පූර්ණ_සංඛ්‍යා main(පූර්ණ_සංඛ්‍යා argc, var_na **argv)
{
	jump_table = NULL;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, parimaa_na(var_na));

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	නම්(NULL != argv[2])
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
	ආපසු EXIT_SUCCESS;
}
