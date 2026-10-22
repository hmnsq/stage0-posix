/* Copyright (C) 2016 Jeremiah Orians
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

#ಅನ್ತರ್ಭಾವ<stdio.h>
#ಅನ್ತರ್ಭಾವ<stdlib.h>
#ಅನ್ತರ್ಭಾವ<stdbool.h>

ಶೂನ್ಯ purge_ಸಾಲು_comments()
{
	ಪೂರ್ಣಾಙ್ಕ c;
	ಕುರು
	{
		c = getchar();

		ಯದಿ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ಯಾವತ್ ( '\n' != c );
}

ಪೂರ್ಣಾಙ್ಕ hex(ಪೂರ್ಣಾಙ್ಕ c)
{
	ಯದಿ (c >= '0' && c <= '9')
	{
		ಪ್ರತ್ಯಾಗಮ (c - 48);
	}
	ಅನ್ಯಥಾ ಯದಿ (c >= 'a' && c <= 'z')
	{
		ಪ್ರತ್ಯಾಗಮ (c - 87);
	}
	ಅನ್ಯಥಾ ಯದಿ (c >= 'A' && c <= 'Z')
	{
		ಪ್ರತ್ಯಾಗಮ (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
	ಪೂರ್ಣಾಙ್ಕ c;
	ಪೂರ್ಣಾಙ್ಕ sum = 0;
	bool toggle;
	toggle = false;

	ಕುರು
	{
		c = getchar();
		ಯದಿ ((c == '#') || (c == ';')) purge_ಸಾಲು_comments();
		ಅನ್ಯಥಾ ಯದಿ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ಯದಿ(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ಅನ್ಯಥಾ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ಯಾವತ್(c != EOF);

	exit(EXIT_SUCCESS);
}
