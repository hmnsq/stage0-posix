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

/* Globals */
FILE* output;

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
શૂન્ય process_byte(અક્ષર c, FILE* source_file)
{
	જો(0 <= hex(c, source_file))
	{
		જો(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
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
		process_byte(c, આગત);
	}
}

/* Standard C main program */
પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
	output = stdout;

	FILE* આગત = fopen(argv[1], "r");
	output = stdout;
	જો(ખાલી != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(આગત);

	/* Set file as executable */
	પાછા EXIT_SUCCESS;
}
