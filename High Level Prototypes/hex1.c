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
	حرف name;
};


/* Globals */
FILE* output;
بنية entry* jump_table;
صحيح ip;

unsigned GetTarget(حرف c)
{
	بنية entry* i;
	لكل(i = jump_table; عدم != i; i = i->next)
	{
		𐡄𐡍(c == i->name)
		{
			أرجع i->target;
		}
	}
	أرجع 0;
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
	entry->name = fgetc(source_file);
	أرجع -1;
}


فراغ outputPointer(صحيح displacement, صحيح number_of_bytes)
{
	unsigned value = displacement;

	طالما(number_of_bytes > 0)
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
	𐡄𐡍(in_set(ch, "%")) ip = ip + 4;
	وإلا exit(EXIT_FAILURE);
}

فراغ storePointer(حرف ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	صحيح target = GetTarget(ch);
	صحيح displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	𐡄𐡍('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	وإلا exit(EXIT_FAILURE);
}

فراغ line_Comment(FILE* source_file)
{
	صحيح c = fgetc(source_file);
	طالما(!in_set(c, "\n\r"))
	{
		𐡄𐡍(EOF == c) اكسر;
		c = fgetc(source_file);
	}
}

صحيح hex(صحيح c, FILE* source_file)
{
	𐡄𐡍 (in_set(c, "0123456789")) أرجع (c - 48);
	وإلا 𐡄𐡍 (in_set(c, "abcdef")) أرجع (c - 87);
	وإلا 𐡄𐡍 (in_set(c, "ABCDEF")) أرجع (c - 55);
	وإلا 𐡄𐡍 (in_set(c, "#;")) line_Comment(source_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* source_file, صحيح اكتب)
{
	𐡄𐡍(0 <= hex(c, source_file))
	{
		𐡄𐡍(toggle)
		{
			𐡄𐡍(اكتب) fputc(((hold * 16)) + hex(c, source_file), output);
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

فراغ first_pass(FILE* input)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		𐡄𐡍(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		𐡄𐡍(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(input);
		}
		وإلا process_byte(c, input, FALSE);
	}
}

فراغ second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		𐡄𐡍(':' == c) c = fgetc(input); /* Deal with : */
		وإلا 𐡄𐡍(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		وإلا process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
صحيح main(صحيح argc, حرف **argv)
{
	jump_table = عدم;
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	𐡄𐡍(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(input);
	rewind(input);

	/* Fix all the references*/
	ip = 0;
	second_pass(input);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
