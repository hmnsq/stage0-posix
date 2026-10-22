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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <unistd.h>
#شامل <sys/stat.h>
#شامل "../M2libc/bootstrappable.h"

#تعریف_کرو max_string 4096

/* Globals */
FILE* output;

خالی سطر_Comment(FILE* ماخذ_file)
{
	صحیح c = fgetc(ماخذ_file);
	جب(!اندر_سیٹ(c, "\n\r"))
	{
		اگر(EOF == c) توڑو;
		c = fgetc(ماخذ_file);
	}
}

صحیح hex(صحیح c, FILE* ماخذ_file)
{
	اگر (اندر_سیٹ(c, "0123456789")) واپس (c - 48);
	ورنہ اگر (اندر_سیٹ(c, "abcdef")) واپس (c - 87);
	ورنہ اگر (اندر_سیٹ(c, "ABCDEF")) واپس (c - 55);
	ورنہ اگر (اندر_سیٹ(c, "#;")) سطر_Comment(ماخذ_file);
	واپس -1;
}

صحیح hold;
صحیح toggle;
خالی process_byte(حرف c, FILE* ماخذ_file)
{
	اگر(0 <= hex(c, ماخذ_file))
	{
		اگر(toggle)
		{
			fputc(((hold * 16)) + hex(c, ماخذ_file), output);
			hold = 0;
		}
		ورنہ
		{
			hold = hex(c, ماخذ_file);
		}
		toggle = !toggle;
	}
}

خالی پہلا_pass(FILE* داخلہ)
{
	toggle = FALSE;
	صحیح c;
	برائے(c = fgetc(داخلہ); EOF != c; c = fgetc(داخلہ))
	{
		process_byte(c, داخلہ);
	}
}

/* Standard C main program */
صحیح مین(صحیح argc, حرف **argv)
{
	output = stdout;

	FILE* داخلہ = fopen(argv[1], "r");
	output = stdout;
	اگر(تہی != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	پہلا_pass(داخلہ);

	/* Set file as executable */
	واپس EXIT_SUCCESS;
}
