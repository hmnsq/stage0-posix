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
	طالما(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		إذا(EOF == c) اكسر;
	}

	أرجع c;
}

صحيح Throwaway_token(FILE* source_file)
{
	صحيح c;
	افعل
	{
		c = fgetc(source_file);
		إذا(EOF == c) اكسر;
	} طالما(!in_set(c, " \t\n>"));

	أرجع c;
}

صحيح length(حرف* s)
{
	صحيح i = 0;
	طالما(0 != s[i]) i = i + 1;
	أرجع i;
}

فراغ Clear_Scratch(حرف* s)
{
	افعل
	{
		s[0] = 0;
		s = s + 1;
	} طالما(0 != s[0]);
}

فراغ Copy_String(حرف* a, حرف* b)
{
	طالما(0 != a[0])
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
		إذا(match(c, i->name))
		{
			أرجع i->target;
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

	أرجع c;
}

فراغ outputPointer(صحيح displacement, صحيح رقم_of_bytes)
{
	unsigned قيمة = displacement;

	طالما(رقم_of_bytes > 0)
	{
		unsigned byte = قيمة % 256;
		قيمة = قيمة / 256;
		fputc(byte, output);
		رقم_of_bytes = رقم_of_bytes - 1;
	}
}

فراغ Update_Pointer(حرف ch)
{
	/* Calculate pointer size*/
	إذا(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	وإلا إذا(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	وإلا إذا('!' == ch) ip = ip + 1; /* Deal with ! */
	وإلا exit(EXIT_FAILURE);
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
	إذا ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	وإلا
	{
		displacement = base - target;
	}

	/* output calculated difference */
	إذا('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	وإلا إذا('$' == ch) outputPointer(target, 2); /* Deal with $ */
	وإلا إذا('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	وإلا إذا('&' == ch) outputPointer(target, 4); /* Deal with & */
	وإلا إذا('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	وإلا exit(EXIT_FAILURE);
}

فراغ سطر_Comment(FILE* source_file)
{
	صحيح c = fgetc(source_file);
	طالما(!in_set(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(source_file);
	}
}

صحيح hex(صحيح c, FILE* source_file)
{
	إذا (in_set(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (in_set(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (in_set(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (in_set(c, "#;")) سطر_Comment(source_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* source_file, صحيح اكتب)
{
	إذا(0 <= hex(c, source_file))
	{
		إذا(toggle)
		{
			إذا(اكتب) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

فراغ first_pass(FILE* مدخل)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(مدخل); EOF != c; c = fgetc(مدخل))
	{
		/* Check for and deal with label */
		إذا(':' == c)
		{
			c = storeLabel(مدخل, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		إذا(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(مدخل);
			إذا ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(مدخل);
			}
		}
		وإلا process_byte(c, مدخل, FALSE);
	}
}

فراغ second_pass(FILE* مدخل)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(مدخل); EOF != c; c = fgetc(مدخل))
	{
		إذا(':' == c) c = Throwaway_token(مدخل); /* Deal with : */
		وإلا إذا(in_set(c, "!@$~%&")) storePointer(c, مدخل);  /* Deal with !, @, $, ~, % and & */
		وإلا process_byte(c, مدخل, TRUE);
	}
}

/* Standard C main program */
صحيح main(صحيح argc, حرف **argv)
{
	jump_table = عدم;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, حجم(حرف));

	FILE* مدخل = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(مدخل);
	rewind(مدخل);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(مدخل);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
