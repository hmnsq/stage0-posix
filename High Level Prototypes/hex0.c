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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ "../M2libc/bootstrappable.h"

#define max_string 4096

/* Globals */
FILE* output;

ಶೂನ್ಯ ಸಾಲು_Comment(FILE* source_file)
{
	ಪೂರ್ಣಾಙ್ಕ c = fgetc(source_file);
	ಯಾವತ್(!in_set(c, "\n\r"))
	{
		ಯದಿ(EOF == c) ಭಙ್ಗ;
		c = fgetc(source_file);
	}
}

ಪೂರ್ಣಾಙ್ಕ hex(ಪೂರ್ಣಾಙ್ಕ c, FILE* source_file)
{
	ಯದಿ (in_set(c, "0123456789")) ಪ್ರತ್ಯಾಗಮ (c - 48);
	ಅನ್ಯಥಾ ಯದಿ (in_set(c, "abcdef")) ಪ್ರತ್ಯಾಗಮ (c - 87);
	ಅನ್ಯಥಾ ಯದಿ (in_set(c, "ABCDEF")) ಪ್ರತ್ಯಾಗಮ (c - 55);
	ಅನ್ಯಥಾ ಯದಿ (in_set(c, "#;")) ಸಾಲು_Comment(source_file);
	ಪ್ರತ್ಯಾಗಮ -1;
}

ಪೂರ್ಣಾಙ್ಕ hold;
ಪೂರ್ಣಾಙ್ಕ toggle;
ಶೂನ್ಯ process_byte(ವರ್ಣ c, FILE* source_file)
{
	ಯದಿ(0 <= hex(c, source_file))
	{
		ಯದಿ(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
			hold = 0;
		}
		ಅನ್ಯಥಾ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

ಶೂನ್ಯ first_pass(FILE* ಒಳಹರಿವು)
{
	toggle = FALSE;
	ಪೂರ್ಣಾಙ್ಕ c;
	ಪ್ರತಿ(c = fgetc(ಒಳಹರಿವು); EOF != c; c = fgetc(ಒಳಹರಿವು))
	{
		process_byte(c, ಒಳಹರಿವು);
	}
}

/* Standard C main program */
ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
	output = stdout;

	FILE* ಒಳಹರಿವು = fopen(argv[1], "r");
	output = stdout;
	ಯದಿ(ರಿಕ್ತ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(ಒಳಹರಿವು);

	/* Set file as executable */
	ಪ್ರತ್ಯಾಗಮ EXIT_SUCCESS;
}
