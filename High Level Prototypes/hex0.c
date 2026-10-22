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

#وصف_ڪريو max_string 4096

/* Globals */
FILE* output;

فراغ سٽ_Comment(FILE* ذريعو_file)
{
	صحيح c = fgetc(ذريعو_file);
	طالما(!اندريون_سيٽ(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(ذريعو_file);
	}
}

صحيح hex(صحيح c, FILE* ذريعو_file)
{
	إذا (اندريون_سيٽ(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (اندريون_سيٽ(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (اندريون_سيٽ(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (اندريون_سيٽ(c, "#;")) سٽ_Comment(ذريعو_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* ذريعو_file)
{
	إذا(0 <= hex(c, ذريعو_file))
	{
		إذا(toggle)
		{
			fputc(((hold * 16)) + hex(c, ذريعو_file), output);
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, ذريعو_file);
		}
		toggle = !toggle;
	}
}

فراغ پهريون_pass(FILE* داخلا)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(داخلا); EOF != c; c = fgetc(داخلا))
	{
		process_byte(c, داخلا);
	}
}

/* Standard C main program */
صحيح مکيه(صحيح argc, حرف **argv)
{
	output = stdout;

	FILE* داخلا = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	پهريون_pass(داخلا);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
