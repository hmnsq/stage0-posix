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

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ "../M2libc/bootstrappable.h"

#define max_string 4096

সংরচনা entry
{
	সংরচনা entry* next;
	unsigned target;
	ৱর্ণ* name;
};


/* Globals */
FILE* output;
সংরচনা entry* jump_table;
পূর্ণাঙ্ক Base_Address;
পূর্ণাঙ্ক ip;
ৱর্ণ* scratch;

পূর্ণাঙ্ক consume_token(FILE* source_file)
{
	পূর্ণাঙ্ক i = 0;
	পূর্ণাঙ্ক c = fgetc(source_file);
	যেতিয়ালৈ(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		যদি(EOF == c) ভঙ্গ;
	}

	ঘূৰাই c;
}

পূর্ণাঙ্ক Throwaway_token(FILE* source_file)
{
	পূর্ণাঙ্ক c;
	কৰা
	{
		c = fgetc(source_file);
		যদি(EOF == c) ভঙ্গ;
	} যেতিয়ালৈ(!in_set(c, " \t\n>"));

	ঘূৰাই c;
}

পূর্ণাঙ্ক length(ৱর্ণ* s)
{
	পূর্ণাঙ্ক i = 0;
	যেতিয়ালৈ(0 != s[i]) i = i + 1;
	ঘূৰাই i;
}

শূন্য Clear_Scratch(ৱর্ণ* s)
{
	কৰা
	{
		s[0] = 0;
		s = s + 1;
	} যেতিয়ালৈ(0 != s[0]);
}

শূন্য Copy_String(ৱর্ণ* a, ৱর্ণ* b)
{
	যেতিয়ালৈ(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(ৱর্ণ* c)
{
	সংরচনা entry* i;
	বাবে(i = jump_table; রিক্ত != i; i = i->next)
	{
		যদি(match(c, i->name))
		{
			ঘূৰাই i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
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
	পূর্ণাঙ্ক c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, পরিমাণ(ৱর্ণ));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ঘূৰাই c;
}

শূন্য outputPointer(পূর্ণাঙ্ক displacement, পূর্ণাঙ্ক সংখ্যা_of_bytes)
{
	unsigned মান = displacement;

	যেতিয়ালৈ(সংখ্যা_of_bytes > 0)
	{
		unsigned byte = মান % 256;
		মান = মান / 256;
		fputc(byte, output);
		সংখ্যা_of_bytes = সংখ্যা_of_bytes - 1;
	}
}

শূন্য Update_Pointer(ৱর্ণ ch)
{
	/* Calculate pointer size*/
	যদি(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	অন্যথা যদি(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	অন্যথা যদি('!' == ch) ip = ip + 1; /* Deal with ! */
	অন্যথা exit(EXIT_FAILURE);
}

শূন্য storePointer(ৱর্ণ ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	পূর্ণাঙ্ক base_sep_p = consume_token(source_file);

	/* Lookup token */
	পূর্ণাঙ্ক target = GetTarget(scratch);
	পূর্ণাঙ্ক displacement;

	পূর্ণাঙ্ক base = ip;

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

শূন্য সাৰি_Comment(FILE* source_file)
{
	পূর্ণাঙ্ক c = fgetc(source_file);
	যেতিয়ালৈ(!in_set(c, "\n\r"))
	{
		যদি(EOF == c) ভঙ্গ;
		c = fgetc(source_file);
	}
}

পূর্ণাঙ্ক hex(পূর্ণাঙ্ক c, FILE* source_file)
{
	যদি (in_set(c, "0123456789")) ঘূৰাই (c - 48);
	অন্যথা যদি (in_set(c, "abcdef")) ঘূৰাই (c - 87);
	অন্যথা যদি (in_set(c, "ABCDEF")) ঘূৰাই (c - 55);
	অন্যথা যদি (in_set(c, "#;")) সাৰি_Comment(source_file);
	ঘূৰাই -1;
}

পূর্ণাঙ্ক hold;
পূর্ণাঙ্ক toggle;
শূন্য process_byte(ৱর্ণ c, FILE* source_file, পূর্ণাঙ্ক লিখ)
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

শূন্য first_pass(FILE* প্ৰবেশ)
{
	toggle = FALSE;
	পূর্ণাঙ্ক c;
	বাবে(c = fgetc(প্ৰবেশ); EOF != c; c = fgetc(প্ৰবেশ))
	{
		/* Check for and deal with label */
		যদি(':' == c)
		{
			c = storeLabel(প্ৰবেশ, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		যদি(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(প্ৰবেশ);
			যদি ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(প্ৰবেশ);
			}
		}
		অন্যথা process_byte(c, প্ৰবেশ, FALSE);
	}
}

শূন্য second_pass(FILE* প্ৰবেশ)
{
	toggle = FALSE;
	hold = 0;

	পূর্ণাঙ্ক c;
	বাবে(c = fgetc(প্ৰবেশ); EOF != c; c = fgetc(প্ৰবেশ))
	{
		যদি(':' == c) c = Throwaway_token(প্ৰবেশ); /* Deal with : */
		অন্যথা যদি(in_set(c, "!@$~%&")) storePointer(c, প্ৰবেশ);  /* Deal with !, @, $, ~, % and & */
		অন্যথা process_byte(c, প্ৰবেশ, TRUE);
	}
}

/* Standard C main program */
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ **argv)
{
	jump_table = রিক্ত;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, পরিমাণ(ৱর্ণ));

	FILE* প্ৰবেশ = fopen(argv[1], "r");
	output = stdout;
	যদি(রিক্ত != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(প্ৰবেশ);
	rewind(প্ৰবেশ);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(প্ৰবেশ);

	/* Set file as executable */
	ঘূৰাই EXIT_SUCCESS;
}
