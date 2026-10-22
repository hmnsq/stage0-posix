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

#অন্তর্ভাব<stdio.h>
#অন্তর্ভাব<stdlib.h>
#অন্তর্ভাব<stdbool.h>

শূন্য purge_সারি_comments()
{
	পূর্ণসংখ্যা c;
	করো
	{
		c = getchar();

		যদি (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} যখন ( '\n' != c );
}

পূর্ণসংখ্যা hex(পূর্ণসংখ্যা c)
{
	যদি (c >= '0' && c <= '9')
	{
		ফেরত (c - 48);
	}
	অন্যথা যদি (c >= 'a' && c <= 'z')
	{
		ফেরত (c - 87);
	}
	অন্যথা যদি (c >= 'A' && c <= 'Z')
	{
		ফেরত (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
	পূর্ণসংখ্যা c;
	পূর্ণসংখ্যা যোগ = 0;
	bool toggle;
	toggle = false;

	করো
	{
		c = getchar();
		যদি ((c == '#') || (c == ';')) purge_সারি_comments();
		অন্যথা যদি ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			যদি(!toggle)
			{
				যোগ = hex(c);
				toggle = true;
			}
			অন্যথা
			{
				যোগ = (যোগ << 4) + hex(c);
				toggle = false;
				fputc(যোগ, stdout);
			}
		}
	}যখন(c != EOF);

	exit(EXIT_SUCCESS);
}
