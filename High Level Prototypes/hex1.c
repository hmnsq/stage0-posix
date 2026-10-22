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
	ଅକ୍ଷର name;
};


/* Globals */
FILE* output;
ସଂରଚନା entry* jump_table;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ip;

unsigned GetTarget(ଅକ୍ଷର c)
{
	ସଂରଚନା entry* i;
	ପାଇଁ(i = jump_table; ଖାଲି != i; i = i->next)
	{
		ଯଦି(c == i->name)
		{
			ଫେରନ୍ତୁ i->target;
		}
	}
	ଫେରନ୍ତୁ 0;
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
	entry->name = fgetc(source_file);
	ଫେରନ୍ତୁ -1;
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
	ଯଦି(in_set(ch, "%")) ip = ip + 4;
	ଅନ୍ୟଥା exit(EXIT_FAILURE);
}

ଶୂନ୍ୟ storePointer(ଅକ୍ଷର ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା target = GetTarget(ch);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	ଯଦି('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
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
		ଯଦି(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(ନିବେଶ);
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
		ଯଦି(':' == c) c = fgetc(ନିବେଶ); /* Deal with : */
		ଅନ୍ୟଥା ଯଦି(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		ଅନ୍ୟଥା process_byte(c, ନିବେଶ, TRUE);
	}
}

/* Standard C main program */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
	jump_table = ଖାଲି;
	output = stdout;

	FILE* ନିବେଶ = fopen(argv[1], "r");
	output = stdout;
	ଯଦି(ଖାଲି != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(ନିବେଶ);
	rewind(ନିବେଶ);

	/* Fix all the references*/
	ip = 0;
	second_pass(ନିବେଶ);

	/* Set file as executable */
	ଫେରନ୍ତୁ EXIT_SUCCESS;
}
