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

#تعریف_کرو max_string 4096

/* Globals */
FILE* output;

فراغ سطر_Comment(FILE* سروت_file)
{
	صحيح c = fgetc(سروت_file);
	طالما(!وچ_سیٹ(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(سروت_file);
	}
}

صحيح hex(صحيح c, FILE* سروت_file)
{
	إذا (وچ_سیٹ(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (وچ_سیٹ(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (وچ_سیٹ(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (وچ_سیٹ(c, "#;")) سطر_Comment(سروت_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* سروت_file)
{
	إذا(0 <= hex(c, سروت_file))
	{
		إذا(toggle)
		{
			fputc(((hold * 16)) + hex(c, سروت_file), output);
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, سروت_file);
		}
		toggle = !toggle;
	}
}

فراغ پہلا_pass(FILE* داخلہ)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(داخلہ); EOF != c; c = fgetc(داخلہ))
	{
		process_byte(c, داخلہ);
	}
}

/* Standard C main program */
صحيح مکھ(صحيح argc, حرف **argv)
{
	output = stdout;

	FILE* داخلہ = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	پہلا_pass(داخلہ);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
