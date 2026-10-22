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

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

void purge_line_comments()
{
	puur_naa_nka c;
	do
	{
		c = getchar();

		ᮚᮦᮔ᮪ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ᮞᮨᮓᮨᮜ ( '\n' != c );
}

puur_naa_nka hex(puur_naa_nka c)
{
	ᮚᮦᮔ᮪ (c >= '0' && c <= '9')
	{
		ᮝᮜᮤ (c - 48);
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (c >= 'a' && c <= 'z')
	{
		ᮝᮜᮤ (c - 87);
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (c >= 'A' && c <= 'Z')
	{
		ᮝᮜᮤ (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

puur_naa_nka main(puur_naa_nka argc, var_na *argv[])
{
	puur_naa_nka c;
	puur_naa_nka sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		ᮚᮦᮔ᮪ ((c == '#') || (c == ';')) purge_line_comments();
		ᮜᮤᮚ ᮚᮦᮔ᮪ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ᮚᮦᮔ᮪(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ᮜᮤᮚ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ᮞᮨᮓᮨᮜ(c != EOF);

	exit(EXIT_SUCCESS);
}
