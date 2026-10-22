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
	ចំនួនគត់ c;
	do
	{
		c = getchar();

		ប្រសិនបើ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ខណៈពេល ( '\n' != c );
}

ចំនួនគត់ hex(ចំនួនគត់ c)
{
	ប្រសិនបើ (c >= '0' && c <= '9')
	{
		ត្រឡប់ (c - 48);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (c >= 'a' && c <= 'z')
	{
		ត្រឡប់ (c - 87);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (c >= 'A' && c <= 'Z')
	{
		ត្រឡប់ (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

ចំនួនគត់ main(ចំនួនគត់ argc, var_na *argv[])
{
	ចំនួនគត់ c;
	ចំនួនគត់ sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		ប្រសិនបើ ((c == '#') || (c == ';')) purge_line_comments();
		បើមិនដូច្នេះ ប្រសិនបើ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ប្រសិនបើ(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			បើមិនដូច្នេះ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ខណៈពេល(c != EOF);

	exit(EXIT_SUCCESS);
}
