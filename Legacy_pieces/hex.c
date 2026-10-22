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
	ကိန်းပြည့် c;
	do
	{
		c = getchar();

		အကယ်၍ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} နေစဉ် ( '\n' != c );
}

ကိန်းပြည့် hex(ကိန်းပြည့် c)
{
	အကယ်၍ (c >= '0' && c <= '9')
	{
		ပြန်ပေး (c - 48);
	}
	မဟုတ်ပါက အကယ်၍ (c >= 'a' && c <= 'z')
	{
		ပြန်ပေး (c - 87);
	}
	မဟုတ်ပါက အကယ်၍ (c >= 'A' && c <= 'Z')
	{
		ပြန်ပေး (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

ကိန်းပြည့် main(ကိန်းပြည့် argc, var_na *argv[])
{
	ကိန်းပြည့် c;
	ကိန်းပြည့် sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		အကယ်၍ ((c == '#') || (c == ';')) purge_line_comments();
		မဟုတ်ပါက အကယ်၍ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			အကယ်၍(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			မဟုတ်ပါက
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}နေစဉ်(c != EOF);

	exit(EXIT_SUCCESS);
}
