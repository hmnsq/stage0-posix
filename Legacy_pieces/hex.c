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
	정수 c;
	do
	{
		c = getchar();

		만약 (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} 동안 ( '\n' != c );
}

정수 hex(정수 c)
{
	만약 (c >= '0' && c <= '9')
	{
		반환 (c - 48);
	}
	아니면 만약 (c >= 'a' && c <= 'z')
	{
		반환 (c - 87);
	}
	아니면 만약 (c >= 'A' && c <= 'Z')
	{
		반환 (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

정수 main(정수 argc, char *argv[])
{
	정수 c;
	정수 sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		만약 ((c == '#') || (c == ';')) purge_line_comments();
		아니면 만약 ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			만약(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			아니면
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}동안(c != EOF);

	exit(EXIT_SUCCESS);
}
