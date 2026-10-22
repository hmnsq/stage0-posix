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

/* Globals */
FILE* output;

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
ଶୂନ୍ୟ process_byte(ଅକ୍ଷର c, FILE* source_file)
{
	ଯଦି(0 <= hex(c, source_file))
	{
		ଯଦି(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
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
		process_byte(c, ନିବେଶ);
	}
}

/* Standard C main program */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
	output = stdout;

	FILE* ନିବେଶ = fopen(argv[1], "r");
	output = stdout;
	ଯଦି(ଖାଲି != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(ନିବେଶ);

	/* Set file as executable */
	ଫେରନ୍ତୁ EXIT_SUCCESS;
}
