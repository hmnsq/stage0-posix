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
	ಪೂರ್ಣಾಂಕ c;
	ಮಾಡು
	{
		c = getchar();

		ಆದರೆ (EOF == c)
		{
			ನಿರ್ಗಮಿಸು(EXIT_SUCCESS);
		}
	} ಆಗ ( '\n' != c );
}

ಪೂರ್ಣಾಂಕ hex(ಪೂರ್ಣಾಂಕ c)
{
	ಆದರೆ (c >= '0' && c <= '9')
	{
		ಹಿಂತಿರುಗಿ (c - 48);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c >= 'a' && c <= 'z')
	{
		ಹಿಂತಿರುಗಿ (c - 87);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c >= 'A' && c <= 'Z')
	{
		ಹಿಂತಿರುಗಿ (c - 55);
	}

	ಬರೆ("You managed to call a hex function without a hex value!!!\n");
	ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
}

ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
	ಪೂರ್ಣಾಂಕ c;
	ಪೂರ್ಣಾಂಕ ಮೊತ್ತ = 0;
	bool toggle;
	toggle = false;

	ಮಾಡು
	{
		c = getchar();
		ಆದರೆ ((c == '#') || (c == ';')) purge_ಸಾಲು_comments();
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ಆದರೆ(!toggle)
			{
				ಮೊತ್ತ = hex(c);
				toggle = true;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
				ಮೊತ್ತ = (ಮೊತ್ತ << 4) + hex(c);
				toggle = false;
				fputc(ಮೊತ್ತ, stdout);
			}
		}
	}ಆಗ(c != EOF);

	ನಿರ್ಗಮಿಸು(EXIT_SUCCESS);
}
