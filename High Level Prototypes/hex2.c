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

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ "../M2libc/bootstrappable.h"

#define max_string 4096

સંરચના entry
{
	સંરચના entry* next;
	unsigned target;
	અક્ષર* name;
};


/* Globals */
FILE* output;
સંરચના entry* jump_table;
પૂર્ણાંક Base_Address;
પૂર્ણાંક ip;
અક્ષર* scratch;

પૂર્ણાંક consume_token(FILE* source_file)
{
	પૂર્ણાંક i = 0;
	પૂર્ણાંક c = fgetc(source_file);
	જ્યારે(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		જો(EOF == c) તોડો;
	}

	પાછા c;
}

પૂર્ણાંક Throwaway_token(FILE* source_file)
{
	પૂર્ણાંક c;
	કરો
	{
		c = fgetc(source_file);
		જો(EOF == c) તોડો;
	} જ્યારે(!in_set(c, " \t\n>"));

	પાછા c;
}

પૂર્ણાંક length(અક્ષર* s)
{
	પૂર્ણાંક i = 0;
	જ્યારે(0 != s[i]) i = i + 1;
	પાછા i;
}

શૂન્ય Clear_Scratch(અક્ષર* s)
{
	કરો
	{
		s[0] = 0;
		s = s + 1;
	} જ્યારે(0 != s[0]);
}

શૂન્ય Copy_String(અક્ષર* a, અક્ષર* b)
{
	જ્યારે(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(અક્ષર* c)
{
	સંરચના entry* i;
	માટે(i = jump_table; ખાલી != i; i = i->next)
	{
		જો(match(c, i->name))
		{
			પાછા i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

પૂર્ણાંક storeLabel(FILE* source_file, પૂર્ણાંક ip)
{
	સંરચના entry* entry = calloc(1, કદ(સંરચના entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	પૂર્ણાંક c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, કદ(અક્ષર));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	પાછા c;
}

શૂન્ય outputPointer(પૂર્ણાંક displacement, પૂર્ણાંક સંખ્યા_of_bytes)
{
	unsigned કિંમત = displacement;

	જ્યારે(સંખ્યા_of_bytes > 0)
	{
		unsigned byte = કિંમત % 256;
		કિંમત = કિંમત / 256;
		fputc(byte, output);
		સંખ્યા_of_bytes = સંખ્યા_of_bytes - 1;
	}
}

શૂન્ય Update_Pointer(અક્ષર ch)
{
	/* Calculate pointer size*/
	જો(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	નહીંતર જો(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	નહીંતર જો('!' == ch) ip = ip + 1; /* Deal with ! */
	નહીંતર exit(EXIT_FAILURE);
}

શૂન્ય storePointer(અક્ષર ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	પૂર્ણાંક base_sep_p = consume_token(source_file);

	/* Lookup token */
	પૂર્ણાંક target = GetTarget(scratch);
	પૂર્ણાંક displacement;

	પૂર્ણાંક base = ip;

	/* Change relative base address to :<base> */
	જો ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	નહીંતર
	{
		displacement = base - target;
	}

	/* output calculated difference */
	જો('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	નહીંતર જો('$' == ch) outputPointer(target, 2); /* Deal with $ */
	નહીંતર જો('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	નહીંતર જો('&' == ch) outputPointer(target, 4); /* Deal with & */
	નહીંતર જો('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	નહીંતર exit(EXIT_FAILURE);
}

શૂન્ય લીટી_Comment(FILE* source_file)
{
	પૂર્ણાંક c = fgetc(source_file);
	જ્યારે(!in_set(c, "\n\r"))
	{
		જો(EOF == c) તોડો;
		c = fgetc(source_file);
	}
}

પૂર્ણાંક hex(પૂર્ણાંક c, FILE* source_file)
{
	જો (in_set(c, "0123456789")) પાછા (c - 48);
	નહીંતર જો (in_set(c, "abcdef")) પાછા (c - 87);
	નહીંતર જો (in_set(c, "ABCDEF")) પાછા (c - 55);
	નહીંતર જો (in_set(c, "#;")) લીટી_Comment(source_file);
	પાછા -1;
}

પૂર્ણાંક hold;
પૂર્ણાંક toggle;
શૂન્ય process_byte(અક્ષર c, FILE* source_file, પૂર્ણાંક લિખ)
{
	જો(0 <= hex(c, source_file))
	{
		જો(toggle)
		{
			જો(લિખ) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		નહીંતર
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

શૂન્ય first_pass(FILE* આગત)
{
	toggle = FALSE;
	પૂર્ણાંક c;
	માટે(c = fgetc(આગત); EOF != c; c = fgetc(આગત))
	{
		/* Check for and deal with label */
		જો(':' == c)
		{
			c = storeLabel(આગત, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		જો(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(આગત);
			જો ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(આગત);
			}
		}
		નહીંતર process_byte(c, આગત, FALSE);
	}
}

શૂન્ય second_pass(FILE* આગત)
{
	toggle = FALSE;
	hold = 0;

	પૂર્ણાંક c;
	માટે(c = fgetc(આગત); EOF != c; c = fgetc(આગત))
	{
		જો(':' == c) c = Throwaway_token(આગત); /* Deal with : */
		નહીંતર જો(in_set(c, "!@$~%&")) storePointer(c, આગત);  /* Deal with !, @, $, ~, % and & */
		નહીંતર process_byte(c, આગત, TRUE);
	}
}

/* Standard C main program */
પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
	jump_table = ખાલી;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, કદ(અક્ષર));

	FILE* આગત = fopen(argv[1], "r");
	output = stdout;
	જો(ખાલી != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(આગત);
	rewind(આગત);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(આગત);

	/* Set file as executable */
	પાછા EXIT_SUCCESS;
}
