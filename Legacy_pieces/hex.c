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

خالی purge_سطر_comments()
{
	صحیح c;
	کرو
	{
		c = getchar();

		اگر (EOF == c)
		{
			باہر_نکلو(EXIT_SUCCESS);
		}
	} جب ( '\n' != c );
}

صحیح hex(صحیح c)
{
	اگر (c >= '0' && c <= '9')
	{
		واپس (c - 48);
	}
	ورنہ اگر (c >= 'a' && c <= 'z')
	{
		واپس (c - 87);
	}
	ورنہ اگر (c >= 'A' && c <= 'Z')
	{
		واپس (c - 55);
	}

	لکھو("You managed to call a hex function without a hex value!!!\n");
	باہر_نکلو(EXIT_FAILURE);
}

صحیح مین(صحیح argc, حرف *argv[])
{
	صحیح c;
	صحیح مجموع = 0;
	bool toggle;
	toggle = false;

	کرو
	{
		c = getchar();
		اگر ((c == '#') || (c == ';')) purge_سطر_comments();
		ورنہ اگر ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			اگر(!toggle)
			{
				مجموع = hex(c);
				toggle = true;
			}
			ورنہ
			{
				مجموع = (مجموع << 4) + hex(c);
				toggle = false;
				fputc(مجموع, stdout);
			}
		}
	}جب(c != EOF);

	باہر_نکلو(EXIT_SUCCESS);
}
