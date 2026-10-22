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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <unistd.h>
#شامل <sys/stat.h>
#شامل "../M2libc/bootstrappable.h"

#define max_string 4096

ساختار entry
{
	ساختار entry* next;
	unsigned target;
	نویسه* name;
};


/* Globals */
FILE* output;
ساختار entry* jump_table;
صحیح Base_Address;
صحیح ip;
نویسه* scratch;

صحیح consume_token(FILE* source_file)
{
	صحیح i = 0;
	صحیح c = fgetc(source_file);
	وقتی(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		اگر(EOF == c) بشکن;
	}

	بازگشت c;
}

صحیح Throwaway_token(FILE* source_file)
{
	صحیح c;
	انجامبده
	{
		c = fgetc(source_file);
		اگر(EOF == c) بشکن;
	} وقتی(!in_set(c, " \t\n>"));

	بازگشت c;
}

صحیح length(نویسه* s)
{
	صحیح i = 0;
	وقتی(0 != s[i]) i = i + 1;
	بازگشت i;
}

تهی Clear_Scratch(نویسه* s)
{
	انجامبده
	{
		s[0] = 0;
		s = s + 1;
	} وقتی(0 != s[0]);
}

تهی Copy_String(نویسه* a, نویسه* b)
{
	وقتی(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(نویسه* c)
{
	ساختار entry* i;
	برای(i = jump_table; پوچ != i; i = i->next)
	{
		اگر(match(c, i->name))
		{
			بازگشت i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

صحیح storeLabel(FILE* source_file, صحیح ip)
{
	ساختار entry* entry = calloc(1, اندازه(ساختار entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	صحیح c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, اندازه(نویسه));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	بازگشت c;
}

تهی outputPointer(صحیح displacement, صحیح عدد_of_bytes)
{
	unsigned مقدار = displacement;

	وقتی(عدد_of_bytes > 0)
	{
		unsigned byte = مقدار % 256;
		مقدار = مقدار / 256;
		fputc(byte, output);
		عدد_of_bytes = عدد_of_bytes - 1;
	}
}

تهی Update_Pointer(نویسه ch)
{
	/* Calculate pointer size*/
	اگر(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	وگرنه اگر(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	وگرنه اگر('!' == ch) ip = ip + 1; /* Deal with ! */
	وگرنه exit(EXIT_FAILURE);
}

تهی storePointer(نویسه ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	صحیح base_sep_p = consume_token(source_file);

	/* Lookup token */
	صحیح target = GetTarget(scratch);
	صحیح displacement;

	صحیح base = ip;

	/* Change relative base address to :<base> */
	اگر ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	وگرنه
	{
		displacement = base - target;
	}

	/* output calculated difference */
	اگر('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	وگرنه اگر('$' == ch) outputPointer(target, 2); /* Deal with $ */
	وگرنه اگر('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	وگرنه اگر('&' == ch) outputPointer(target, 4); /* Deal with & */
	وگرنه اگر('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	وگرنه exit(EXIT_FAILURE);
}

تهی سطر_Comment(FILE* source_file)
{
	صحیح c = fgetc(source_file);
	وقتی(!in_set(c, "\n\r"))
	{
		اگر(EOF == c) بشکن;
		c = fgetc(source_file);
	}
}

صحیح hex(صحیح c, FILE* source_file)
{
	اگر (in_set(c, "0123456789")) بازگشت (c - 48);
	وگرنه اگر (in_set(c, "abcdef")) بازگشت (c - 87);
	وگرنه اگر (in_set(c, "ABCDEF")) بازگشت (c - 55);
	وگرنه اگر (in_set(c, "#;")) سطر_Comment(source_file);
	بازگشت -1;
}

صحیح hold;
صحیح toggle;
تهی process_byte(نویسه c, FILE* source_file, صحیح اكتب)
{
	اگر(0 <= hex(c, source_file))
	{
		اگر(toggle)
		{
			اگر(اكتب) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		وگرنه
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

تهی first_pass(FILE* درون_داد)
{
	toggle = FALSE;
	صحیح c;
	برای(c = fgetc(درون_داد); EOF != c; c = fgetc(درون_داد))
	{
		/* Check for and deal with label */
		اگر(':' == c)
		{
			c = storeLabel(درون_داد, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		اگر(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(درون_داد);
			اگر ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(درون_داد);
			}
		}
		وگرنه process_byte(c, درون_داد, FALSE);
	}
}

تهی second_pass(FILE* درون_داد)
{
	toggle = FALSE;
	hold = 0;

	صحیح c;
	برای(c = fgetc(درون_داد); EOF != c; c = fgetc(درون_داد))
	{
		اگر(':' == c) c = Throwaway_token(درون_داد); /* Deal with : */
		وگرنه اگر(in_set(c, "!@$~%&")) storePointer(c, درون_داد);  /* Deal with !, @, $, ~, % and & */
		وگرنه process_byte(c, درون_داد, TRUE);
	}
}

/* Standard C main program */
صحیح اصلی(صحیح argc, نویسه **argv)
{
	jump_table = پوچ;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, اندازه(نویسه));

	FILE* درون_داد = fopen(argv[1], "r");
	output = stdout;
	اگر(پوچ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(درون_داد);
	rewind(درون_داد);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(درون_داد);

	/* Set file as executable */
	بازگشت EXIT_SUCCESS;
}
