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
	نویسه name;
};


/* Globals */
FILE* output;
ساختار entry* jump_table;
صحیح ip;

unsigned GetTarget(نویسه c)
{
	ساختار entry* i;
	برای(i = jump_table; پوچ != i; i = i->next)
	{
		اگر(c == i->name)
		{
			بازگشت i->target;
		}
	}
	بازگشت 0;
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
	entry->name = fgetc(source_file);
	بازگشت -1;
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
	اگر(in_set(ch, "%")) ip = ip + 4;
	وگرنه exit(EXIT_FAILURE);
}

تهی storePointer(نویسه ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	صحیح target = GetTarget(ch);
	صحیح displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	اگر('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
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
		اگر(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(درون_داد);
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
		اگر(':' == c) c = fgetc(درون_داد); /* Deal with : */
		وگرنه اگر(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		وگرنه process_byte(c, درون_داد, TRUE);
	}
}

/* Standard C main program */
صحیح اصلی(صحیح argc, نویسه **argv)
{
	jump_table = پوچ;
	output = stdout;

	FILE* درون_داد = fopen(argv[1], "r");
	output = stdout;
	اگر(پوچ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(درون_داد);
	rewind(درون_داد);

	/* Fix all the references*/
	ip = 0;
	second_pass(درون_داد);

	/* Set file as executable */
	بازگشت EXIT_SUCCESS;
}
