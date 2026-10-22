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

		ᱡᱩᱫᱤ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ᱡᱚᱠᱷᱚᱱ ( '\n' != c );
}

puur_naa_nka hex(puur_naa_nka c)
{
	ᱡᱩᱫᱤ (c >= '0' && c <= '9')
	{
		ᱨᱩᱣᱟᱹᱲ (c - 48);
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (c >= 'a' && c <= 'z')
	{
		ᱨᱩᱣᱟᱹᱲ (c - 87);
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (c >= 'A' && c <= 'Z')
	{
		ᱨᱩᱣᱟᱹᱲ (c - 55);
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
		ᱡᱩᱫᱤ ((c == '#') || (c == ';')) purge_line_comments();
		ᱵᱟᱝ ᱡᱩᱫᱤ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ᱡᱩᱫᱤ(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ᱵᱟᱝ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ᱡᱚᱠᱷᱚᱱ(c != EOF);

	exit(EXIT_SUCCESS);
}
