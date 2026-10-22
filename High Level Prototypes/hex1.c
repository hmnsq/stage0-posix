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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব "../M2libc/bootstrappable.h"

#define max_string 4096

সংরচনা entry
{
	সংরচনা entry* next;
	unsigned target;
	বর্ণ name;
};


/* Globals */
FILE* output;
সংরচনা entry* jump_table;
পূর্ণাঙ্ক ip;

unsigned GetTarget(বর্ণ c)
{
	সংরচনা entry* i;
	প্রতি(i = jump_table; রিক্ত != i; i = i->next)
	{
		যদি(c == i->name)
		{
			প্রত্যাগম i->target;
		}
	}
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক storeLabel(FILE* source_file, পূর্ণাঙ্ক ip)
{
	সংরচনা entry* entry = calloc(1, পরিমাণ(সংরচনা entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	প্রত্যাগম -1;
}


শূন্য outputPointer(পূর্ণাঙ্ক displacement, পূর্ণাঙ্ক সংখ্যা_of_bytes)
{
	unsigned মান = displacement;

	যাবত্(সংখ্যা_of_bytes > 0)
	{
		unsigned byte = মান % 256;
		মান = মান / 256;
		fputc(byte, output);
		সংখ্যা_of_bytes = সংখ্যা_of_bytes - 1;
	}
}

শূন্য Update_Pointer(বর্ণ ch)
{
	/* Calculate pointer size*/
	যদি(in_set(ch, "%")) ip = ip + 4;
	অন্যথা exit(EXIT_FAILURE);
}

শূন্য storePointer(বর্ণ ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	পূর্ণাঙ্ক target = GetTarget(ch);
	পূর্ণাঙ্ক displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	যদি('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	অন্যথা exit(EXIT_FAILURE);
}

শূন্য সারি_Comment(FILE* source_file)
{
	পূর্ণাঙ্ক c = fgetc(source_file);
	যাবত্(!in_set(c, "\n\r"))
	{
		যদি(EOF == c) ভঙ্গ;
		c = fgetc(source_file);
	}
}

পূর্ণাঙ্ক hex(পূর্ণাঙ্ক c, FILE* source_file)
{
	যদি (in_set(c, "0123456789")) প্রত্যাগম (c - 48);
	অন্যথা যদি (in_set(c, "abcdef")) প্রত্যাগম (c - 87);
	অন্যথা যদি (in_set(c, "ABCDEF")) প্রত্যাগম (c - 55);
	অন্যথা যদি (in_set(c, "#;")) সারি_Comment(source_file);
	প্রত্যাগম -1;
}

পূর্ণাঙ্ক hold;
পূর্ণাঙ্ক toggle;
শূন্য process_byte(বর্ণ c, FILE* source_file, পূর্ণাঙ্ক লিখ)
{
	যদি(0 <= hex(c, source_file))
	{
		যদি(toggle)
		{
			যদি(লিখ) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		অন্যথা
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

শূন্য first_pass(FILE* প্রবেশ)
{
	toggle = FALSE;
	পূর্ণাঙ্ক c;
	প্রতি(c = fgetc(প্রবেশ); EOF != c; c = fgetc(প্রবেশ))
	{
		/* Check for and deal with label */
		যদি(':' == c)
		{
			c = storeLabel(প্রবেশ, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		যদি(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(প্রবেশ);
		}
		অন্যথা process_byte(c, প্রবেশ, FALSE);
	}
}

শূন্য second_pass(FILE* প্রবেশ)
{
	toggle = FALSE;
	hold = 0;

	পূর্ণাঙ্ক c;
	প্রতি(c = fgetc(প্রবেশ); EOF != c; c = fgetc(প্রবেশ))
	{
		যদি(':' == c) c = fgetc(প্রবেশ); /* Deal with : */
		অন্যথা যদি(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		অন্যথা process_byte(c, প্রবেশ, TRUE);
	}
}

/* Standard C main program */
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, বর্ণ **argv)
{
	jump_table = রিক্ত;
	output = stdout;

	FILE* প্রবেশ = fopen(argv[1], "r");
	output = stdout;
	যদি(রিক্ত != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(প্রবেশ);
	rewind(প্রবেশ);

	/* Fix all the references*/
	ip = 0;
	second_pass(প্রবেশ);

	/* Set file as executable */
	প্রত্যাগম EXIT_SUCCESS;
}
