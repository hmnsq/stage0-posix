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
	จำนวนเต็ม c;
	do
	{
		c = getchar();

		ถ้า (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ขณะ ( '\n' != c );
}

จำนวนเต็ม hex(จำนวนเต็ม c)
{
	ถ้า (c >= '0' && c <= '9')
	{
		คืน (c - 48);
	}
	มิฉะนั้น ถ้า (c >= 'a' && c <= 'z')
	{
		คืน (c - 87);
	}
	มิฉะนั้น ถ้า (c >= 'A' && c <= 'Z')
	{
		คืน (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

จำนวนเต็ม หลัก(จำนวนเต็ม argc, var_na *argv[])
{
	จำนวนเต็ม c;
	จำนวนเต็ม sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		ถ้า ((c == '#') || (c == ';')) purge_line_comments();
		มิฉะนั้น ถ้า ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ถ้า(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			มิฉะนั้น
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ขณะ(c != EOF);

	exit(EXIT_SUCCESS);
}
