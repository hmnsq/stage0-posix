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
	ཧྲིལ c;
	do
	{
		c = getchar();

		གལ་ཏེ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} སྐབས ( '\n' != c );
}

ཧྲིལ hex(ཧྲིལ c)
{
	གལ་ཏེ (c >= '0' && c <= '9')
	{
		ཕྱིར (c - 48);
	}
	དེ་མིན གལ་ཏེ (c >= 'a' && c <= 'z')
	{
		ཕྱིར (c - 87);
	}
	དེ་མིན གལ་ཏེ (c >= 'A' && c <= 'Z')
	{
		ཕྱིར (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

ཧྲིལ གཙོ(ཧྲིལ argc, var_na *argv[])
{
	ཧྲིལ c;
	ཧྲིལ sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		གལ་ཏེ ((c == '#') || (c == ';')) purge_line_comments();
		དེ་མིན གལ་ཏེ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			གལ་ཏེ(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			དེ་མིན
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}སྐབས(c != EOF);

	exit(EXIT_SUCCESS);
}
