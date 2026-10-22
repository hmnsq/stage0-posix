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
		إذا(c == i->name)
		{
			إرجاع i->target;
		}
	}
	إرجاع 0;
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
	إرجاع -1;
}


فراغ outputPointer(صحيح displacement, صحيح العدد_of_bytes)
{
	unsigned قيمة = displacement;

	بينما(العدد_of_bytes > 0)
	{
		unsigned byte = قيمة % 256;
		قيمة = قيمة / 256;
		fputc(byte, output);
		العدد_of_bytes = العدد_of_bytes - 1;
	}
}

فراغ Update_Pointer(حرف ch)
{
	/* Calculate pointer size*/
	إذا(in_set(ch, "%")) ip = ip + 4;
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
	إذا('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	وإلا exit(EXIT_FAILURE);
}

فراغ سطر_Comment(FILE* source_file)
{
	صحيح c = fgetc(source_file);
	بينما(!in_set(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(source_file);
	}
}

صحيح hex(صحيح c, FILE* source_file)
{
	إذا (in_set(c, "0123456789")) إرجاع (c - 48);
	وإلا إذا (in_set(c, "abcdef")) إرجاع (c - 87);
	وإلا إذا (in_set(c, "ABCDEF")) إرجاع (c - 55);
	وإلا إذا (in_set(c, "#;")) سطر_Comment(source_file);
	إرجاع -1;
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

فراغ first_pass(FILE* إدخال)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(إدخال); EOF != c; c = fgetc(إدخال))
	{
		/* Check for and deal with label */
		إذا(':' == c)
		{
			c = storeLabel(إدخال, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		إذا(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(إدخال);
		}
		وإلا process_byte(c, إدخال, FALSE);
	}
}

فراغ second_pass(FILE* إدخال)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(إدخال); EOF != c; c = fgetc(إدخال))
	{
		إذا(':' == c) c = fgetc(إدخال); /* Deal with : */
		وإلا إذا(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		وإلا process_byte(c, إدخال, TRUE);
	}
}

/* Standard C main program */
صحيح الرئيسية(صحيح argc, حرف **argv)
{
	jump_table = عدم;
	output = stdout;

	FILE* إدخال = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(إدخال);
	rewind(إدخال);

	/* Fix all the references*/
	ip = 0;
	second_pass(إدخال);

	/* Set file as executable */
	إرجاع EXIT_SUCCESS;
}
