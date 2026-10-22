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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ "../M2libc/bootstrappable.h"

#define max_string 4096

ಸಂರಚನಾ entry
{
	ಸಂರಚನಾ entry* next;
	unsigned target;
	ವರ್ಣ* name;
};


/* Globals */
FILE* output;
ಸಂರಚನಾ entry* jump_table;
ಪೂರ್ಣಾಙ್ಕ Base_Address;
ಪೂರ್ಣಾಙ್ಕ ip;
ವರ್ಣ* scratch;

ಪೂರ್ಣಾಙ್ಕ consume_token(FILE* source_file)
{
	ಪೂರ್ಣಾಙ್ಕ i = 0;
	ಪೂರ್ಣಾಙ್ಕ c = fgetc(source_file);
	ಯಾವತ್(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		ಯದಿ(EOF == c) ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ c;
}

ಪೂರ್ಣಾಙ್ಕ Throwaway_token(FILE* source_file)
{
	ಪೂರ್ಣಾಙ್ಕ c;
	ಕುರು
	{
		c = fgetc(source_file);
		ಯದಿ(EOF == c) ಭಙ್ಗ;
	} ಯಾವತ್(!in_set(c, " \t\n>"));

	ಪ್ರತ್ಯಾಗಮ c;
}

ಪೂರ್ಣಾಙ್ಕ length(ವರ್ಣ* s)
{
	ಪೂರ್ಣಾಙ್ಕ i = 0;
	ಯಾವತ್(0 != s[i]) i = i + 1;
	ಪ್ರತ್ಯಾಗಮ i;
}

ಶೂನ್ಯ Clear_Scratch(ವರ್ಣ* s)
{
	ಕುರು
	{
		s[0] = 0;
		s = s + 1;
	} ಯಾವತ್(0 != s[0]);
}

ಶೂನ್ಯ Copy_String(ವರ್ಣ* a, ವರ್ಣ* b)
{
	ಯಾವತ್(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(ವರ್ಣ* c)
{
	ಸಂರಚನಾ entry* i;
	ಪ್ರತಿ(i = jump_table; ರಿಕ್ತ != i; i = i->next)
	{
		ಯದಿ(match(c, i->name))
		{
			ಪ್ರತ್ಯಾಗಮ i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

ಪೂರ್ಣಾಙ್ಕ storeLabel(FILE* source_file, ಪೂರ್ಣಾಙ್ಕ ip)
{
	ಸಂರಚನಾ entry* entry = calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	ಪೂರ್ಣಾಙ್ಕ c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, ಪರಿಮಾಣ(ವರ್ಣ));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ಪ್ರತ್ಯಾಗಮ c;
}

ಶೂನ್ಯ outputPointer(ಪೂರ್ಣಾಙ್ಕ displacement, ಪೂರ್ಣಾಙ್ಕ ಸಂಖ್ಯೆ_of_bytes)
{
	unsigned ಮೌಲ್ಯ = displacement;

	ಯಾವತ್(ಸಂಖ್ಯೆ_of_bytes > 0)
	{
		unsigned byte = ಮೌಲ್ಯ % 256;
		ಮೌಲ್ಯ = ಮೌಲ್ಯ / 256;
		fputc(byte, output);
		ಸಂಖ್ಯೆ_of_bytes = ಸಂಖ್ಯೆ_of_bytes - 1;
	}
}

ಶೂನ್ಯ Update_Pointer(ವರ್ಣ ch)
{
	/* Calculate pointer size*/
	ಯದಿ(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	ಅನ್ಯಥಾ ಯದಿ(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	ಅನ್ಯಥಾ ಯದಿ('!' == ch) ip = ip + 1; /* Deal with ! */
	ಅನ್ಯಥಾ exit(EXIT_FAILURE);
}

ಶೂನ್ಯ storePointer(ವರ್ಣ ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	ಪೂರ್ಣಾಙ್ಕ base_sep_p = consume_token(source_file);

	/* Lookup token */
	ಪೂರ್ಣಾಙ್ಕ target = GetTarget(scratch);
	ಪೂರ್ಣಾಙ್ಕ displacement;

	ಪೂರ್ಣಾಙ್ಕ base = ip;

	/* Change relative base address to :<base> */
	ಯದಿ ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	ಅನ್ಯಥಾ
	{
		displacement = base - target;
	}

	/* output calculated difference */
	ಯದಿ('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	ಅನ್ಯಥಾ ಯದಿ('$' == ch) outputPointer(target, 2); /* Deal with $ */
	ಅನ್ಯಥಾ ಯದಿ('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	ಅನ್ಯಥಾ ಯದಿ('&' == ch) outputPointer(target, 4); /* Deal with & */
	ಅನ್ಯಥಾ ಯದಿ('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	ಅನ್ಯಥಾ exit(EXIT_FAILURE);
}

ಶೂನ್ಯ ಸಾಲು_Comment(FILE* source_file)
{
	ಪೂರ್ಣಾಙ್ಕ c = fgetc(source_file);
	ಯಾವತ್(!in_set(c, "\n\r"))
	{
		ಯದಿ(EOF == c) ಭಙ್ಗ;
		c = fgetc(source_file);
	}
}

ಪೂರ್ಣಾಙ್ಕ hex(ಪೂರ್ಣಾಙ್ಕ c, FILE* source_file)
{
	ಯದಿ (in_set(c, "0123456789")) ಪ್ರತ್ಯಾಗಮ (c - 48);
	ಅನ್ಯಥಾ ಯದಿ (in_set(c, "abcdef")) ಪ್ರತ್ಯಾಗಮ (c - 87);
	ಅನ್ಯಥಾ ಯದಿ (in_set(c, "ABCDEF")) ಪ್ರತ್ಯಾಗಮ (c - 55);
	ಅನ್ಯಥಾ ಯದಿ (in_set(c, "#;")) ಸಾಲು_Comment(source_file);
	ಪ್ರತ್ಯಾಗಮ -1;
}

ಪೂರ್ಣಾಙ್ಕ hold;
ಪೂರ್ಣಾಙ್ಕ toggle;
ಶೂನ್ಯ process_byte(ವರ್ಣ c, FILE* source_file, ಪೂರ್ಣಾಙ್ಕ ಲಿಖ)
{
	ಯದಿ(0 <= hex(c, source_file))
	{
		ಯದಿ(toggle)
		{
			ಯದಿ(ಲಿಖ) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ಅನ್ಯಥಾ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

ಶೂನ್ಯ first_pass(FILE* ಒಳಹರಿವು)
{
	toggle = FALSE;
	ಪೂರ್ಣಾಙ್ಕ c;
	ಪ್ರತಿ(c = fgetc(ಒಳಹರಿವು); EOF != c; c = fgetc(ಒಳಹರಿವು))
	{
		/* Check for and deal with label */
		ಯದಿ(':' == c)
		{
			c = storeLabel(ಒಳಹರಿವು, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ಯದಿ(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(ಒಳಹರಿವು);
			ಯದಿ ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(ಒಳಹರಿವು);
			}
		}
		ಅನ್ಯಥಾ process_byte(c, ಒಳಹರಿವು, FALSE);
	}
}

ಶೂನ್ಯ second_pass(FILE* ಒಳಹರಿವು)
{
	toggle = FALSE;
	hold = 0;

	ಪೂರ್ಣಾಙ್ಕ c;
	ಪ್ರತಿ(c = fgetc(ಒಳಹರಿವು); EOF != c; c = fgetc(ಒಳಹರಿವು))
	{
		ಯದಿ(':' == c) c = Throwaway_token(ಒಳಹರಿವು); /* Deal with : */
		ಅನ್ಯಥಾ ಯದಿ(in_set(c, "!@$~%&")) storePointer(c, ಒಳಹರಿವು);  /* Deal with !, @, $, ~, % and & */
		ಅನ್ಯಥಾ process_byte(c, ಒಳಹರಿವು, TRUE);
	}
}

/* Standard C main program */
ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
	jump_table = ರಿಕ್ತ;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, ಪರಿಮಾಣ(ವರ್ಣ));

	FILE* ಒಳಹರಿವು = fopen(argv[1], "r");
	output = stdout;
	ಯದಿ(ರಿಕ್ತ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(ಒಳಹರಿವು);
	rewind(ಒಳಹರಿವು);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(ಒಳಹರಿವು);

	/* Set file as executable */
	ಪ್ರತ್ಯಾಗಮ EXIT_SUCCESS;
}
