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
	цяло c;
	do
	{
		c = getchar();

		ако (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} докато ( '\n' != c );
}

цяло hex(цяло c)
{
	ако (c >= '0' && c <= '9')
	{
		върни (c - 48);
	}
	иначе ако (c >= 'a' && c <= 'z')
	{
		върни (c - 87);
	}
	иначе ако (c >= 'A' && c <= 'Z')
	{
		върни (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

цяло main(цяло argc, char *argv[])
{
	цяло c;
	цяло sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		ако ((c == '#') || (c == ';')) purge_line_comments();
		иначе ако ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ако(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			иначе
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}докато(c != EOF);

	exit(EXIT_SUCCESS);
}
