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

কাঠামো entry
{
	কাঠামো entry* পরবর্তী;
	unsigned target;
	অক্ষর* name;
};


/* Globals */
FILE* output;
কাঠামো entry* jump_table;
পূর্ণসংখ্যা Base_Address;
পূর্ণসংখ্যা ip;
অক্ষর* scratch;

পূর্ণসংখ্যা consume_token(FILE* source_file)
{
	পূর্ণসংখ্যা i = 0;
	পূর্ণসংখ্যা c = fgetc(source_file);
	যখন(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		যদি(EOF == c) বিরতি;
	}

	ফেরত c;
}

পূর্ণসংখ্যা Throwaway_token(FILE* source_file)
{
	পূর্ণসংখ্যা c;
	করো
	{
		c = fgetc(source_file);
		যদি(EOF == c) বিরতি;
	} যখন(!in_set(c, " \t\n>"));

	ফেরত c;
}

পূর্ণসংখ্যা দৈর্ঘ্য(অক্ষর* s)
{
	পূর্ণসংখ্যা i = 0;
	যখন(0 != s[i]) i = i + 1;
	ফেরত i;
}

শূন্য Clear_Scratch(অক্ষর* s)
{
	করো
	{
		s[0] = 0;
		s = s + 1;
	} যখন(0 != s[0]);
}

শূন্য Copy_String(অক্ষর* a, অক্ষর* b)
{
	যখন(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(অক্ষর* c)
{
	কাঠামো entry* i;
	জন্য(i = jump_table; ফাঁকা != i; i = i->পরবর্তী)
	{
		যদি(match(c, i->name))
		{
			ফেরত i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

পূর্ণসংখ্যা storeLabel(FILE* source_file, পূর্ণসংখ্যা ip)
{
	কাঠামো entry* entry = calloc(1, আকার(কাঠামো entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->পরবর্তী = jump_table;
	jump_table = entry;

	/* Store string */
	পূর্ণসংখ্যা c = consume_token(source_file);
	entry->name = calloc(দৈর্ঘ্য(scratch) + 1, আকার(অক্ষর));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ফেরত c;
}

শূন্য outputPointer(পূর্ণসংখ্যা displacement, পূর্ণসংখ্যা সংখ্যা_of_bytes)
{
	unsigned মান = displacement;

	যখন(সংখ্যা_of_bytes > 0)
	{
		unsigned byte = মান % 256;
		মান = মান / 256;
		fputc(byte, output);
		সংখ্যা_of_bytes = সংখ্যা_of_bytes - 1;
	}
}

শূন্য Update_Pointer(অক্ষর ch)
{
	/* Calculate pointer size*/
	যদি(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	অন্যথা যদি(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	অন্যথা যদি('!' == ch) ip = ip + 1; /* Deal with ! */
	অন্যথা exit(EXIT_FAILURE);
}

শূন্য storePointer(অক্ষর ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	পূর্ণসংখ্যা base_sep_p = consume_token(source_file);

	/* Lookup token */
	পূর্ণসংখ্যা target = GetTarget(scratch);
	পূর্ণসংখ্যা displacement;

	পূর্ণসংখ্যা base = ip;

	/* Change relative base address to :<base> */
	যদি ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	অন্যথা
	{
		displacement = base - target;
	}

	/* output calculated difference */
	যদি('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	অন্যথা যদি('$' == ch) outputPointer(target, 2); /* Deal with $ */
	অন্যথা যদি('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	অন্যথা যদি('&' == ch) outputPointer(target, 4); /* Deal with & */
	অন্যথা যদি('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	অন্যথা exit(EXIT_FAILURE);
}

শূন্য সারি_Comment(FILE* source_file)
{
	পূর্ণসংখ্যা c = fgetc(source_file);
	যখন(!in_set(c, "\n\r"))
	{
		যদি(EOF == c) বিরতি;
		c = fgetc(source_file);
	}
}

পূর্ণসংখ্যা hex(পূর্ণসংখ্যা c, FILE* source_file)
{
	যদি (in_set(c, "0123456789")) ফেরত (c - 48);
	অন্যথা যদি (in_set(c, "abcdef")) ফেরত (c - 87);
	অন্যথা যদি (in_set(c, "ABCDEF")) ফেরত (c - 55);
	অন্যথা যদি (in_set(c, "#;")) সারি_Comment(source_file);
	ফেরত -1;
}

পূর্ণসংখ্যা hold;
পূর্ণসংখ্যা toggle;
শূন্য process_byte(অক্ষর c, FILE* source_file, পূর্ণসংখ্যা লিখ)
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
	পূর্ণসংখ্যা c;
	জন্য(c = fgetc(প্রবেশ); EOF != c; c = fgetc(প্রবেশ))
	{
		/* Check for and deal with label */
		যদি(':' == c)
		{
			c = storeLabel(প্রবেশ, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		যদি(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(প্রবেশ);
			যদি ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(প্রবেশ);
			}
		}
		অন্যথা process_byte(c, প্রবেশ, FALSE);
	}
}

শূন্য second_pass(FILE* প্রবেশ)
{
	toggle = FALSE;
	hold = 0;

	পূর্ণসংখ্যা c;
	জন্য(c = fgetc(প্রবেশ); EOF != c; c = fgetc(প্রবেশ))
	{
		যদি(':' == c) c = Throwaway_token(প্রবেশ); /* Deal with : */
		অন্যথা যদি(in_set(c, "!@$~%&")) storePointer(c, প্রবেশ);  /* Deal with !, @, $, ~, % and & */
		অন্যথা process_byte(c, প্রবেশ, TRUE);
	}
}

/* Standard C main program */
পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
	jump_table = ফাঁকা;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, আকার(অক্ষর));

	FILE* প্রবেশ = fopen(argv[1], "r");
	output = stdout;
	যদি(ফাঁকা != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(প্রবেশ);
	rewind(প্রবেশ);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(প্রবেশ);

	/* Set file as executable */
	ফেরত EXIT_SUCCESS;
}
