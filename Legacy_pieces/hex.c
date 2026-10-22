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
	số_nguyên c;
	do
	{
		c = getchar();

		nếu (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} trong_khi ( '\n' != c );
}

số_nguyên hex(số_nguyên c)
{
	nếu (c >= '0' && c <= '9')
	{
		trả_về (c - 48);
	}
	không_thì nếu (c >= 'a' && c <= 'z')
	{
		trả_về (c - 87);
	}
	không_thì nếu (c >= 'A' && c <= 'Z')
	{
		trả_về (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

số_nguyên main(số_nguyên argc, char *argv[])
{
	số_nguyên c;
	số_nguyên sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		nếu ((c == '#') || (c == ';')) purge_line_comments();
		không_thì nếu ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			nếu(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			không_thì
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}trong_khi(c != EOF);

	exit(EXIT_SUCCESS);
}
