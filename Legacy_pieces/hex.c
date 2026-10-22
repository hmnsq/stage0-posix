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
	целое c;
	do
	{
		c = getchar();

		если (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} пока ( '\n' != c );
}

целое hex(целое c)
{
	если (c >= '0' && c <= '9')
	{
		вернуть (c - 48);
	}
	иначе если (c >= 'a' && c <= 'z')
	{
		вернуть (c - 87);
	}
	иначе если (c >= 'A' && c <= 'Z')
	{
		вернуть (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

целое основной(целое argc, char *argv[])
{
	целое c;
	целое sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		если ((c == '#') || (c == ';')) purge_line_comments();
		иначе если ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			если(!toggle)
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
	}пока(c != EOF);

	exit(EXIT_SUCCESS);
}
