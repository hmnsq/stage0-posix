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

ಪೂರ್ಣಾಙ್ಕ main (ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ** argv)
{
	ಯದಿ(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		exit(EXIT_FAILURE);
	}

	FILE* in = fopen(argv[1], "r");
	ಯದಿ(ರಿಕ್ತ == in)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		exit(EXIT_FAILURE);
	}

	ವರ್ಣ* table = "0123456789ABCDEF";
	ಪೂರ್ಣಾಙ್ಕ c = fgetc(in);
	ಪೂರ್ಣಾಙ್ಕ col = 40;
	ಯಾವತ್(EOF != c)
	{
		fputc(table[c / 16], stdout);
		fputc(table[c % 16], stdout);
		col = col - 2;
		ಯದಿ(0 == col)
		{
			col = 40;
			fputc('\n', stdout);
		}
		ಅನ್ಯಥಾ fputc(' ', stdout);
		c = fgetc(in);
	}
	fputc('\n', stdout);
	exit(EXIT_SUCCESS);
}
