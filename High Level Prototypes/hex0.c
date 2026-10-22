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

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ "../M2libc/bootstrappable.h"

#define max_string 4096

/* Globals */
FILE* output;

வெறுமை வரி_Comment(FILE* source_file)
{
	முழு c = fgetc(source_file);
	போது(!in_set(c, "\n\r"))
	{
		எனில்(EOF == c) நிறுத்து;
		c = fgetc(source_file);
	}
}

முழு hex(முழு c, FILE* source_file)
{
	எனில் (in_set(c, "0123456789")) திரும்பு (c - 48);
	இல்லையெனில் எனில் (in_set(c, "abcdef")) திரும்பு (c - 87);
	இல்லையெனில் எனில் (in_set(c, "ABCDEF")) திரும்பு (c - 55);
	இல்லையெனில் எனில் (in_set(c, "#;")) வரி_Comment(source_file);
	திரும்பு -1;
}

முழு hold;
முழு toggle;
வெறுமை process_byte(எழுத்து c, FILE* source_file)
{
	எனில்(0 <= hex(c, source_file))
	{
		எனில்(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		இல்லையெனில்
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

வெறுமை first_pass(FILE* உள்ளீடு)
{
	toggle = FALSE;
	முழு c;
	க்கு(c = fgetc(உள்ளீடு); EOF != c; c = fgetc(உள்ளீடு))
	{
		process_byte(c, உள்ளீடு);
	}
}

/* Standard C main program */
முழு முதன்மை(முழு argc, எழுத்து **argv)
{
	output = stdout;

	FILE* உள்ளீடு = fopen(argv[1], "r");
	output = stdout;
	எனில்(வெற்று != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(உள்ளீடு);

	/* Set file as executable */
	திரும்பு EXIT_SUCCESS;
}
