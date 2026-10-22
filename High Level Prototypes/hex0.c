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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "../M2libc/bootstrappable.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ max_string 4096

/* Globals */
FILE* output;

ਖਾਲੀ ਸਤਰ_Comment(FILE* ਸਰੋਤ_file)
{
	ਪੂਰਨ c = fgetc(ਸਰੋਤ_file);
	ਜਦੋਂ(!ਵਿੱਚ_ਸੈੱਟ(c, "\n\r"))
	{
		ਜੇ(EOF == c) ਰੋਕੋ;
		c = fgetc(ਸਰੋਤ_file);
	}
}

ਪੂਰਨ hex(ਪੂਰਨ c, FILE* ਸਰੋਤ_file)
{
	ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "0123456789")) ਵਾਪਸ (c - 48);
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "abcdef")) ਵਾਪਸ (c - 87);
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "ABCDEF")) ਵਾਪਸ (c - 55);
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "#;")) ਸਤਰ_Comment(ਸਰੋਤ_file);
	ਵਾਪਸ -1;
}

ਪੂਰਨ hold;
ਪੂਰਨ toggle;
ਖਾਲੀ process_byte(ਅੱਖਰ c, FILE* ਸਰੋਤ_file)
{
	ਜੇ(0 <= hex(c, ਸਰੋਤ_file))
	{
		ਜੇ(toggle)
		{
			fputc(((hold * 16)) + hex(c, ਸਰੋਤ_file), output);
			hold = 0;
		}
		ਨਹੀਂਤਾਂ
		{
			hold = hex(c, ਸਰੋਤ_file);
		}
		toggle = !toggle;
	}
}

ਖਾਲੀ ਪਹਿਲਾ_pass(FILE* ਨਿਵੇਸ਼)
{
	toggle = FALSE;
	ਪੂਰਨ c;
	ਲਈ(c = fgetc(ਨਿਵੇਸ਼); EOF != c; c = fgetc(ਨਿਵੇਸ਼))
	{
		process_byte(c, ਨਿਵੇਸ਼);
	}
}

/* Standard C main program */
ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
	output = stdout;

	FILE* ਨਿਵੇਸ਼ = fopen(argv[1], "r");
	output = stdout;
	ਜੇ(ਨਿਰਾ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	ਪਹਿਲਾ_pass(ਨਿਵੇਸ਼);

	/* Set file as executable */
	ਵਾਪਸ EXIT_SUCCESS;
}
