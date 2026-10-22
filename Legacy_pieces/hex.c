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
	цео c;
	do
	{
		c = getchar();

		ако (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} док ( '\n' != c );
}

цео hex(цео c)
{
	ако (c >= '0' && c <= '9')
	{
		врати (c - 48);
	}
	инако ако (c >= 'a' && c <= 'z')
	{
		врати (c - 87);
	}
	инако ако (c >= 'A' && c <= 'Z')
	{
		врати (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

цео main(цео argc, char *argv[])
{
	цео c;
	цео sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		ако ((c == '#') || (c == ';')) purge_line_comments();
		инако ако ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ако(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			инако
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}док(c != EOF);

	exit(EXIT_SUCCESS);
}
