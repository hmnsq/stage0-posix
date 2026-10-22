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
	පූර්ණ_සංඛ්‍යා c;
	do
	{
		c = getchar();

		නම් (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} අතරතුර ( '\n' != c );
}

පූර්ණ_සංඛ්‍යා hex(පූර්ණ_සංඛ්‍යා c)
{
	නම් (c >= '0' && c <= '9')
	{
		ආපසු (c - 48);
	}
	නැතහොත් නම් (c >= 'a' && c <= 'z')
	{
		ආපසු (c - 87);
	}
	නැතහොත් නම් (c >= 'A' && c <= 'Z')
	{
		ආපසු (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

පූර්ණ_සංඛ්‍යා main(පූර්ණ_සංඛ්‍යා argc, var_na *argv[])
{
	පූර්ණ_සංඛ්‍යා c;
	පූර්ණ_සංඛ්‍යා sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		නම් ((c == '#') || (c == ';')) purge_line_comments();
		නැතහොත් නම් ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			නම්(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			නැතහොත්
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}අතරතුර(c != EOF);

	exit(EXIT_SUCCESS);
}
