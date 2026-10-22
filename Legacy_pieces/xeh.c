/* Copyright (C) 2016-2021 Jeremiah Orians
 * This file is part of stage0.
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

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ (ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ** argv)
{
	ಆದರೆ(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	}

	FILE* ಒಳ = fopen(argv[1], "r");
	ಆದರೆ(ಖಾಲಿ == ಒಳ)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	}

	ಅಕ್ಷರ* ಕೋಷ್ಟಕ = "0123456789ABCDEF";
	ಪೂರ್ಣಾಂಕ c = fgetc(ಒಳ);
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 40;
	ಆಗ(EOF != c)
	{
		fputc(ಕೋಷ್ಟಕ[c / 16], stdout);
		fputc(ಕೋಷ್ಟಕ[c % 16], stdout);
		ಲಂಬಸಾಲು = ಲಂಬಸಾಲು - 2;
		ಆದರೆ(0 == ಲಂಬಸಾಲು)
		{
			ಲಂಬಸಾಲು = 40;
			fputc('\n', stdout);
		}
		ಇಲ್ಲದಿದ್ದರೆ fputc(' ', stdout);
		c = fgetc(ಒಳ);
	}
	fputc('\n', stdout);
	ನಿರ್ಗಮಿಸು(EXIT_SUCCESS);
}
