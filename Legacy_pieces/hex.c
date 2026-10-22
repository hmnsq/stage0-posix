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
	整数 c;
	do
	{
		c = getchar();

		もし (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ながら ( '\n' != c );
}

整数 hex(整数 c)
{
	もし (c >= '0' && c <= '9')
	{
		戻る (c - 48);
	}
	否则 もし (c >= 'a' && c <= 'z')
	{
		戻る (c - 87);
	}
	否则 もし (c >= 'A' && c <= 'Z')
	{
		戻る (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

整数 メインページ(整数 argc, char *argv[])
{
	整数 c;
	整数 sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		もし ((c == '#') || (c == ';')) purge_line_comments();
		否则 もし ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			もし(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			否则
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ながら(c != EOF);

	exit(EXIT_SUCCESS);
}
