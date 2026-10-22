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

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <unistd.h>
#ضمن <sys/stat.h>
#ضمن "../M2libc/bootstrappable.h"

#define max_string 4096

بنية entry
{
	بنية entry* next;
	unsigned target;
	حرف* name;
};


/* Globals */
FILE* output;
بنية entry* jump_table;
صحيح Base_Address;
صحيح ip;
حرف* scratch;

صحيح consume_token(FILE* source_file)
{
	صحيح i = 0;
	صحيح c = fgetc(source_file);
	ܟܡܐ(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		ܐܢ(EOF == c) اكسر;
	}

	ܦܢܐ c;
}

صحيح Throwaway_token(FILE* source_file)
{
	صحيح c;
	ܥܒܕ
	{
		c = fgetc(source_file);
		ܐܢ(EOF == c) اكسر;
	} ܟܡܐ(!in_set(c, " \t\n>"));

	ܦܢܐ c;
}

صحيح length(حرف* s)
{
	صحيح i = 0;
	ܟܡܐ(0 != s[i]) i = i + 1;
	ܦܢܐ i;
}

فراغ Clear_Scratch(حرف* s)
{
	ܥܒܕ
	{
		s[0] = 0;
		s = s + 1;
	} ܟܡܐ(0 != s[0]);
}

فراغ Copy_String(حرف* a, حرف* b)
{
	ܟܡܐ(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(حرف* c)
{
	بنية entry* i;
	لكل(i = jump_table; عدم != i; i = i->next)
	{
		ܐܢ(match(c, i->name))
		{
			ܦܢܐ i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

صحيح storeLabel(FILE* source_file, صحيح ip)
{
	بنية entry* entry = calloc(1, حجم(بنية entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	صحيح c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, حجم(حرف));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ܦܢܐ c;
}

فراغ outputPointer(صحيح displacement, صحيح number_of_bytes)
{
	unsigned value = displacement;

	ܟܡܐ(number_of_bytes > 0)
	{
		unsigned byte = value % 256;
		value = value / 256;
		fputc(byte, output);
		number_of_bytes = number_of_bytes - 1;
	}
}

فراغ Update_Pointer(حرف ch)
{
	/* Calculate pointer size*/
	ܐܢ(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	ܐܚܪܢܐ ܐܢ(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	ܐܚܪܢܐ ܐܢ('!' == ch) ip = ip + 1; /* Deal with ! */
	ܐܚܪܢܐ exit(EXIT_FAILURE);
}

فراغ storePointer(حرف ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	صحيح base_sep_p = consume_token(source_file);

	/* Lookup token */
	صحيح target = GetTarget(scratch);
	صحيح displacement;

	صحيح base = ip;

	/* Change relative base address to :<base> */
	ܐܢ ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	ܐܚܪܢܐ
	{
		displacement = base - target;
	}

	/* output calculated difference */
	ܐܢ('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	ܐܚܪܢܐ ܐܢ('$' == ch) outputPointer(target, 2); /* Deal with $ */
	ܐܚܪܢܐ ܐܢ('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	ܐܚܪܢܐ ܐܢ('&' == ch) outputPointer(target, 4); /* Deal with & */
	ܐܚܪܢܐ ܐܢ('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	ܐܚܪܢܐ exit(EXIT_FAILURE);
}

فراغ line_Comment(FILE* source_file)
{
	صحيح c = fgetc(source_file);
	ܟܡܐ(!in_set(c, "\n\r"))
	{
		ܐܢ(EOF == c) اكسر;
		c = fgetc(source_file);
	}
}

صحيح hex(صحيح c, FILE* source_file)
{
	ܐܢ (in_set(c, "0123456789")) ܦܢܐ (c - 48);
	ܐܚܪܢܐ ܐܢ (in_set(c, "abcdef")) ܦܢܐ (c - 87);
	ܐܚܪܢܐ ܐܢ (in_set(c, "ABCDEF")) ܦܢܐ (c - 55);
	ܐܚܪܢܐ ܐܢ (in_set(c, "#;")) line_Comment(source_file);
	ܦܢܐ -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* source_file, صحيح اكتب)
{
	ܐܢ(0 <= hex(c, source_file))
	{
		ܐܢ(toggle)
		{
			ܐܢ(اكتب) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ܐܚܪܢܐ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

فراغ first_pass(FILE* input)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		ܐܢ(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ܐܢ(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(input);
			ܐܢ ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(input);
			}
		}
		ܐܚܪܢܐ process_byte(c, input, FALSE);
	}
}

فراغ second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		ܐܢ(':' == c) c = Throwaway_token(input); /* Deal with : */
		ܐܚܪܢܐ ܐܢ(in_set(c, "!@$~%&")) storePointer(c, input);  /* Deal with !, @, $, ~, % and & */
		ܐܚܪܢܐ process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
صحيح main(صحيح argc, حرف **argv)
{
	jump_table = عدم;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, حجم(حرف));

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ܐܢ(عدم != argv[2])
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
	ܦܢܐ EXIT_SUCCESS;
}
