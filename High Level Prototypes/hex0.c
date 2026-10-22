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

#ವ್ಯಾಖ್ಯಾನಿಸು max_string 4096

/* Globals */
FILE* output;

ಶೂನ್ಯ ಸಾಲು_Comment(FILE* ಮೂಲ_file)
{
	ಪೂರ್ಣಾಂಕ c = fgetc(ಮೂಲ_file);
	ಆಗ(!ಒಳ_ಗಣ(c, "\n\r"))
	{
		ಆದರೆ(EOF == c) ನಿಲ್ಲಿಸು;
		c = fgetc(ಮೂಲ_file);
	}
}

ಪೂರ್ಣಾಂಕ hex(ಪೂರ್ಣಾಂಕ c, FILE* ಮೂಲ_file)
{
	ಆದರೆ (ಒಳ_ಗಣ(c, "0123456789")) ಹಿಂತಿರುಗಿ (c - 48);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(c, "abcdef")) ಹಿಂತಿರುಗಿ (c - 87);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(c, "ABCDEF")) ಹಿಂತಿರುಗಿ (c - 55);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(c, "#;")) ಸಾಲು_Comment(ಮೂಲ_file);
	ಹಿಂತಿರುಗಿ -1;
}

ಪೂರ್ಣಾಂಕ hold;
ಪೂರ್ಣಾಂಕ toggle;
ಶೂನ್ಯ process_byte(ಅಕ್ಷರ c, FILE* ಮೂಲ_file)
{
	ಆದರೆ(0 <= hex(c, ಮೂಲ_file))
	{
		ಆದರೆ(toggle)
		{
			fputc(((hold * 16)) + hex(c, ಮೂಲ_file), output);
			hold = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			hold = hex(c, ಮೂಲ_file);
		}
		toggle = !toggle;
	}
}

ಶೂನ್ಯ ಮೊದಲ_pass(FILE* ಒಳಹರಿವು)
{
	toggle = FALSE;
	ಪೂರ್ಣಾಂಕ c;
	ಗಾಗಿ(c = fgetc(ಒಳಹರಿವು); EOF != c; c = fgetc(ಒಳಹರಿವು))
	{
		process_byte(c, ಒಳಹರಿವು);
	}
}

/* Standard C main program */
ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
	output = stdout;

	FILE* ಒಳಹರಿವು = fopen(argv[1], "r");
	output = stdout;
	ಆದರೆ(ಖಾಲಿ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	ಮೊದಲ_pass(ಒಳಹರಿವು);

	/* Set file as executable */
	ಹಿಂತಿರುಗಿ EXIT_SUCCESS;
}
