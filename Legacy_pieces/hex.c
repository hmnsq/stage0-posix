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

#ضمن<stdio.h>
#ضمن<stdlib.h>
#ضمن<stdbool.h>

فراغ purge_لي_comments()
{
	صحيح c;
	افعل
	{
		c = getchar();

		إذا (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} طالما ( '\n' != c );
}

صحيح hex(صحيح c)
{
	إذا (c >= '0' && c <= '9')
	{
		أرجع (c - 48);
	}
	وإلا إذا (c >= 'a' && c <= 'z')
	{
		أرجع (c - 87);
	}
	وإلا إذا (c >= 'A' && c <= 'Z')
	{
		أرجع (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

صحيح main(صحيح argc, حرف *argv[])
{
	صحيح c;
	صحيح sum = 0;
	bool toggle;
	toggle = false;

	افعل
	{
		c = getchar();
		إذا ((c == '#') || (c == ';')) purge_لي_comments();
		وإلا إذا ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			إذا(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			وإلا
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}طالما(c != EOF);

	exit(EXIT_SUCCESS);
}
