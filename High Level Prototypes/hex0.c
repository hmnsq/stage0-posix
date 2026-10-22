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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ "../M2libc/bootstrappable.h"

#నిర్వచించు max_string 4096

/* Globals */
FILE* output;

శూన్య పంక్తి_Comment(FILE* ఆధారం_file)
{
	పూర్ణాంకం c = fgetc(ఆధారం_file);
	అయితే(!లోపల_సమితి(c, "\n\r"))
	{
		ఒకవేళ(EOF == c) ఆపు;
		c = fgetc(ఆధారం_file);
	}
}

పూర్ణాంకం hex(పూర్ణాంకం c, FILE* ఆధారం_file)
{
	ఒకవేళ (లోపల_సమితి(c, "0123456789")) తిరిగి (c - 48);
	లేకపోతే ఒకవేళ (లోపల_సమితి(c, "abcdef")) తిరిగి (c - 87);
	లేకపోతే ఒకవేళ (లోపల_సమితి(c, "ABCDEF")) తిరిగి (c - 55);
	లేకపోతే ఒకవేళ (లోపల_సమితి(c, "#;")) పంక్తి_Comment(ఆధారం_file);
	తిరిగి -1;
}

పూర్ణాంకం hold;
పూర్ణాంకం toggle;
శూన్య process_byte(అక్షరం c, FILE* ఆధారం_file)
{
	ఒకవేళ(0 <= hex(c, ఆధారం_file))
	{
		ఒకవేళ(toggle)
		{
			fputc(((hold * 16)) + hex(c, ఆధారం_file), output);
			hold = 0;
		}
		లేకపోతే
		{
			hold = hex(c, ఆధారం_file);
		}
		toggle = !toggle;
	}
}

శూన్య మొదటి_pass(FILE* ఆదానం)
{
	toggle = FALSE;
	పూర్ణాంకం c;
	కొరకు(c = fgetc(ఆదానం); EOF != c; c = fgetc(ఆదానం))
	{
		process_byte(c, ఆదానం);
	}
}

/* Standard C main program */
పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
	output = stdout;

	FILE* ఆదానం = fopen(argv[1], "r");
	output = stdout;
	ఒకవేళ(ఖాళీ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	మొదటి_pass(ఆదానం);

	/* Set file as executable */
	తిరిగి EXIT_SUCCESS;
}
