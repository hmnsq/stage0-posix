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

#অন্তর্ভাৱ<stdio.h>
#অন্তর্ভাৱ<stdlib.h>
#অন্তর্ভাৱ<stdbool.h>

শূন্য purge_সাৰি_comments()
{
	পূর্ণাঙ্ক c;
	কৰা
	{
		c = getchar();

		যদি (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} যেতিয়ালৈ ( '\n' != c );
}

পূর্ণাঙ্ক hex(পূর্ণাঙ্ক c)
{
	যদি (c >= '0' && c <= '9')
	{
		ঘূৰাই (c - 48);
	}
	অন্যথা যদি (c >= 'a' && c <= 'z')
	{
		ঘূৰাই (c - 87);
	}
	অন্যথা যদি (c >= 'A' && c <= 'Z')
	{
		ঘূৰাই (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
	পূর্ণাঙ্ক c;
	পূর্ণাঙ্ক sum = 0;
	bool toggle;
	toggle = false;

	কৰা
	{
		c = getchar();
		যদি ((c == '#') || (c == ';')) purge_সাৰি_comments();
		অন্যথা যদি ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			যদি(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			অন্যথা
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}যেতিয়ালৈ(c != EOF);

	exit(EXIT_SUCCESS);
}
