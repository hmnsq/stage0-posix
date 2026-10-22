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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ "../M2libc/bootstrappable.h"

#define max_string 4096

ସଂରଚନା entry
{
	ସଂରଚନା entry* next;
	unsigned target;
	ଅକ୍ଷର* name;
};


/* Globals */
FILE* output;
ସଂରଚନା entry* jump_table;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Base_Address;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ip;
ଅକ୍ଷର* scratch;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା consume_token(FILE* source_file)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = fgetc(source_file);
	ଯେତେବେଳେ(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		ଯଦି(EOF == c) ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ c;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Throwaway_token(FILE* source_file)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	କର
	{
		c = fgetc(source_file);
		ଯଦି(EOF == c) ଭାଙ୍ଗ;
	} ଯେତେବେଳେ(!in_set(c, " \t\n>"));

	ଫେରନ୍ତୁ c;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା length(ଅକ୍ଷର* s)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
	ଯେତେବେଳେ(0 != s[i]) i = i + 1;
	ଫେରନ୍ତୁ i;
}

ଶୂନ୍ୟ Clear_Scratch(ଅକ୍ଷର* s)
{
	କର
	{
		s[0] = 0;
		s = s + 1;
	} ଯେତେବେଳେ(0 != s[0]);
}

ଶୂନ୍ୟ Copy_String(ଅକ୍ଷର* a, ଅକ୍ଷର* b)
{
	ଯେତେବେଳେ(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(ଅକ୍ଷର* c)
{
	ସଂରଚନା entry* i;
	ପାଇଁ(i = jump_table; ଖାଲି != i; i = i->next)
	{
		ଯଦି(match(c, i->name))
		{
			ଫେରନ୍ତୁ i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା storeLabel(FILE* source_file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ip)
{
	ସଂରଚନା entry* entry = calloc(1, ଆକାର(ସଂରଚନା entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, ଆକାର(ଅକ୍ଷର));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	ଫେରନ୍ତୁ c;
}

ଶୂନ୍ୟ outputPointer(ପୂର୍ଣ୍ଣସଂଖ୍ୟା displacement, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସଂଖ୍ୟା_of_bytes)
{
	unsigned ମୂଲ୍ୟ = displacement;

	ଯେତେବେଳେ(ସଂଖ୍ୟା_of_bytes > 0)
	{
		unsigned byte = ମୂଲ୍ୟ % 256;
		ମୂଲ୍ୟ = ମୂଲ୍ୟ / 256;
		fputc(byte, output);
		ସଂଖ୍ୟା_of_bytes = ସଂଖ୍ୟା_of_bytes - 1;
	}
}

ଶୂନ୍ୟ Update_Pointer(ଅକ୍ଷର ch)
{
	/* Calculate pointer size*/
	ଯଦି(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	ଅନ୍ୟଥା ଯଦି(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	ଅନ୍ୟଥା ଯଦି('!' == ch) ip = ip + 1; /* Deal with ! */
	ଅନ୍ୟଥା exit(EXIT_FAILURE);
}

ଶୂନ୍ୟ storePointer(ଅକ୍ଷର ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_sep_p = consume_token(source_file);

	/* Lookup token */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା target = GetTarget(scratch);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା displacement;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = ip;

	/* Change relative base address to :<base> */
	ଯଦି ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	ଅନ୍ୟଥା
	{
		displacement = base - target;
	}

	/* output calculated difference */
	ଯଦି('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	ଅନ୍ୟଥା ଯଦି('$' == ch) outputPointer(target, 2); /* Deal with $ */
	ଅନ୍ୟଥା ଯଦି('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	ଅନ୍ୟଥା ଯଦି('&' == ch) outputPointer(target, 4); /* Deal with & */
	ଅନ୍ୟଥା ଯଦି('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	ଅନ୍ୟଥା exit(EXIT_FAILURE);
}

ଶୂନ୍ୟ ଧାଡ଼ି_Comment(FILE* source_file)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = fgetc(source_file);
	ଯେତେବେଳେ(!in_set(c, "\n\r"))
	{
		ଯଦି(EOF == c) ଭାଙ୍ଗ;
		c = fgetc(source_file);
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hex(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, FILE* source_file)
{
	ଯଦି (in_set(c, "0123456789")) ଫେରନ୍ତୁ (c - 48);
	ଅନ୍ୟଥା ଯଦି (in_set(c, "abcdef")) ଫେରନ୍ତୁ (c - 87);
	ଅନ୍ୟଥା ଯଦି (in_set(c, "ABCDEF")) ଫେରନ୍ତୁ (c - 55);
	ଅନ୍ୟଥା ଯଦି (in_set(c, "#;")) ଧାଡ଼ି_Comment(source_file);
	ଫେରନ୍ତୁ -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hold;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା toggle;
ଶୂନ୍ୟ process_byte(ଅକ୍ଷର c, FILE* source_file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଲିଖ)
{
	ଯଦି(0 <= hex(c, source_file))
	{
		ଯଦି(toggle)
		{
			ଯଦି(ଲିଖ) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ଅନ୍ୟଥା
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

ଶୂନ୍ୟ first_pass(FILE* ନିବେଶ)
{
	toggle = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	ପାଇଁ(c = fgetc(ନିବେଶ); EOF != c; c = fgetc(ନିବେଶ))
	{
		/* Check for and deal with label */
		ଯଦି(':' == c)
		{
			c = storeLabel(ନିବେଶ, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ଯଦି(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(ନିବେଶ);
			ଯଦି ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(ନିବେଶ);
			}
		}
		ଅନ୍ୟଥା process_byte(c, ନିବେଶ, FALSE);
	}
}

ଶୂନ୍ୟ second_pass(FILE* ନିବେଶ)
{
	toggle = FALSE;
	hold = 0;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	ପାଇଁ(c = fgetc(ନିବେଶ); EOF != c; c = fgetc(ନିବେଶ))
	{
		ଯଦି(':' == c) c = Throwaway_token(ନିବେଶ); /* Deal with : */
		ଅନ୍ୟଥା ଯଦି(in_set(c, "!@$~%&")) storePointer(c, ନିବେଶ);  /* Deal with !, @, $, ~, % and & */
		ଅନ୍ୟଥା process_byte(c, ନିବେଶ, TRUE);
	}
}

/* Standard C main program */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
	jump_table = ଖାଲି;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, ଆକାର(ଅକ୍ଷର));

	FILE* ନିବେଶ = fopen(argv[1], "r");
	output = stdout;
	ଯଦି(ଖାଲି != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(ନିବେଶ);
	rewind(ନିବେଶ);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(ନିବେଶ);

	/* Set file as executable */
	ଫେରନ୍ତୁ EXIT_SUCCESS;
}
