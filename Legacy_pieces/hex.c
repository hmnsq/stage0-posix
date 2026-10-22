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

#شامل<stdio.h>
#شامل<stdlib.h>
#شامل<stdbool.h>

تهی purge_سطر_comments()
{
	صحیح c;
	انجامبده
	{
		c = getchar();

		اگر (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} وقتی ( '\n' != c );
}

صحیح hex(صحیح c)
{
	اگر (c >= '0' && c <= '9')
	{
		بازگشت (c - 48);
	}
	وگرنه اگر (c >= 'a' && c <= 'z')
	{
		بازگشت (c - 87);
	}
	وگرنه اگر (c >= 'A' && c <= 'Z')
	{
		بازگشت (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

صحیح اصلی(صحیح argc, نویسه *argv[])
{
	صحیح c;
	صحیح sum = 0;
	bool toggle;
	toggle = false;

	انجامبده
	{
		c = getchar();
		اگر ((c == '#') || (c == ';')) purge_سطر_comments();
		وگرنه اگر ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			اگر(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			وگرنه
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}وقتی(c != EOF);

	exit(EXIT_SUCCESS);
}
