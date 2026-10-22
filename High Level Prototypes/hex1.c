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

ರಚನೆ ನಮೂದು
{
	ರಚನೆ ನಮೂದು* ಮುಂದಿನ;
	ಚಿಹ್ನೆರಹಿತ ಗುರಿ;
	ಅಕ್ಷರ name;
};


/* Globals */
FILE* output;
ರಚನೆ ನಮೂದು* jump_ಕೋಷ್ಟಕ;
ಪೂರ್ಣಾಂಕ ip;

ಚಿಹ್ನೆರಹಿತ Get_ಗುರಿ(ಅಕ್ಷರ c)
{
	ರಚನೆ ನಮೂದು* i;
	ಗಾಗಿ(i = jump_ಕೋಷ್ಟಕ; ಖಾಲಿ != i; i = i->ಮುಂದಿನ)
	{
		ಆದರೆ(c == i->name)
		{
			ಹಿಂತಿರುಗಿ i->ಗುರಿ;
		}
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ storeLabel(FILE* ಮೂಲ_file, ಪೂರ್ಣಾಂಕ ip)
{
	ರಚನೆ ನಮೂದು* ನಮೂದು = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(1, ಗಾತ್ರ(ರಚನೆ ನಮೂದು));

	/* Ensure we have target address */
	ನಮೂದು->ಗುರಿ = ip;

	/* Prepend to list */
	ನಮೂದು->ಮುಂದಿನ = jump_ಕೋಷ್ಟಕ;
	jump_ಕೋಷ್ಟಕ = ನಮೂದು;

	/* Store string */
	ನಮೂದು->name = fgetc(ಮೂಲ_file);
	ಹಿಂತಿರುಗಿ -1;
}


ಶೂನ್ಯ output_ಸೂಚಕ(ಪೂರ್ಣಾಂಕ displacement, ಪೂರ್ಣಾಂಕ ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು)
{
	ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ = displacement;

	ಆಗ(ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು > 0)
	{
		ಚಿಹ್ನೆರಹಿತ byte = ಮೌಲ್ಯ % 256;
		ಮೌಲ್ಯ = ಮೌಲ್ಯ / 256;
		fputc(byte, output);
		ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು = ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು - 1;
	}
}

ಶೂನ್ಯ Update_ಸೂಚಕ(ಅಕ್ಷರ ch)
{
	/* Calculate pointer size*/
	ಆದರೆ(ಒಳ_ಗಣ(ch, "%")) ip = ip + 4;
	ಇಲ್ಲದಿದ್ದರೆ ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
}

ಶೂನ್ಯ store_ಸೂಚಕ(ಅಕ್ಷರ ch)
{
	/* Get string of pointer */
	Update_ಸೂಚಕ(ch);

	/* Lookup token */
	ಪೂರ್ಣಾಂಕ ಗುರಿ = Get_ಗುರಿ(ch);
	ಪೂರ್ಣಾಂಕ displacement;

	/* Change relative base address to :<base> */
	displacement = (ಗುರಿ - ip);

	/* output calculated difference */
	ಆದರೆ('%' == ch) output_ಸೂಚಕ(displacement, 4);  /* Deal with % */
	ಇಲ್ಲದಿದ್ದರೆ ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
}

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
ಶೂನ್ಯ process_byte(ಅಕ್ಷರ c, FILE* ಮೂಲ_file, ಪೂರ್ಣಾಂಕ ಲಿಖ)
{
	ಆದರೆ(0 <= hex(c, ಮೂಲ_file))
	{
		ಆದರೆ(toggle)
		{
			ಆದರೆ(ಲಿಖ) fputc(((hold * 16)) + hex(c, ಮೂಲ_file), output);
			ip = ip + 1;
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
		/* Check for and deal with label */
		ಆದರೆ(':' == c)
		{
			c = storeLabel(ಒಳಹರಿವು, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ಆದರೆ(ಒಳ_ಗಣ(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_ಸೂಚಕ(c);
			c = fgetc(ಒಳಹರಿವು);
		}
		ಇಲ್ಲದಿದ್ದರೆ process_byte(c, ಒಳಹರಿವು, FALSE);
	}
}

ಶೂನ್ಯ ಎರಡನೇ_pass(FILE* ಒಳಹರಿವು)
{
	toggle = FALSE;
	hold = 0;

	ಪೂರ್ಣಾಂಕ c;
	ಗಾಗಿ(c = fgetc(ಒಳಹರಿವು); EOF != c; c = fgetc(ಒಳಹರಿವು))
	{
		ಆದರೆ(':' == c) c = fgetc(ಒಳಹರಿವು); /* Deal with : */
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಒಳ_ಗಣ(c, "%")) store_ಸೂಚಕ(c);  /* Deal with %*/
		ಇಲ್ಲದಿದ್ದರೆ process_byte(c, ಒಳಹರಿವು, TRUE);
	}
}

/* Standard C main program */
ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
	jump_ಕೋಷ್ಟಕ = ಖಾಲಿ;
	output = stdout;

	FILE* ಒಳಹರಿವು = fopen(argv[1], "r");
	output = stdout;
	ಆದರೆ(ಖಾಲಿ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	ಮೊದಲ_pass(ಒಳಹರಿವು);
	rewind(ಒಳಹರಿವು);

	/* Fix all the references*/
	ip = 0;
	ಎರಡನೇ_pass(ಒಳಹರಿವು);

	/* Set file as executable */
	ಹಿಂತಿರುಗಿ EXIT_SUCCESS;
}
