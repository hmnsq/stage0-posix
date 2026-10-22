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

#تعریف_کړه max_string 4096

/* Globals */
FILE* output;

فراغ کرښه_Comment(FILE* سرچینه_file)
{
	صحيح c = fgetc(سرچینه_file);
	طالما(!دننه_سېټ(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(سرچینه_file);
	}
}

صحيح hex(صحيح c, FILE* سرچینه_file)
{
	إذا (دننه_سېټ(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (دننه_سېټ(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (دننه_سېټ(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (دننه_سېټ(c, "#;")) کرښه_Comment(سرچینه_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* سرچینه_file)
{
	إذا(0 <= hex(c, سرچینه_file))
	{
		إذا(toggle)
		{
			fputc(((hold * 16)) + hex(c, سرچینه_file), output);
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, سرچینه_file);
		}
		toggle = !toggle;
	}
}

فراغ لومړی_pass(FILE* ننوت)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(ننوت); EOF != c; c = fgetc(ننوت))
	{
		process_byte(c, ننوت);
	}
}

/* Standard C main program */
صحيح اصلي(صحيح argc, حرف **argv)
{
	output = stdout;

	FILE* ننوت = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	لومړی_pass(ننوت);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
