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

		ᤀᤡ᤺ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ᤀᤠ᤺ ( '\n' != c );
}

puur_naa_nka hex(puur_naa_nka c)
{
	ᤀᤡ᤺ (c >= '0' && c <= '9')
	{
		ᤀᤡᤴ (c - 48);
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c >= 'a' && c <= 'z')
	{
		ᤀᤡᤴ (c - 87);
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (c >= 'A' && c <= 'Z')
	{
		ᤀᤡᤴ (c - 55);
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
		ᤀᤡ᤺ ((c == '#') || (c == ';')) purge_line_comments();
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ᤀᤡ᤺(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ᤀᤡ᤺ᤆᤡ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ᤀᤠ᤺(c != EOF);

	exit(EXIT_SUCCESS);
}
