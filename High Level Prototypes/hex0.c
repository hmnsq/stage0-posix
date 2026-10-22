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

/* Globals */
FILE* output;

فراغ line_Comment(FILE* source_file)
{
	صحيح c = fgetc(source_file);
	طالما(!in_set(c, "\n\r"))
	{
		𐡄𐡍(EOF == c) اكسر;
		c = fgetc(source_file);
	}
}

صحيح hex(صحيح c, FILE* source_file)
{
	𐡄𐡍 (in_set(c, "0123456789")) أرجع (c - 48);
	وإلا 𐡄𐡍 (in_set(c, "abcdef")) أرجع (c - 87);
	وإلا 𐡄𐡍 (in_set(c, "ABCDEF")) أرجع (c - 55);
	وإلا 𐡄𐡍 (in_set(c, "#;")) line_Comment(source_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* source_file)
{
	𐡄𐡍(0 <= hex(c, source_file))
	{
		𐡄𐡍(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

فراغ first_pass(FILE* input)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		process_byte(c, input);
	}
}

/* Standard C main program */
صحيح main(صحيح argc, حرف **argv)
{
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	𐡄𐡍(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(input);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
