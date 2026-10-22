/* Copyright (C) 2016-2021 Jeremiah Orians
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

#شامل <stdlib.h>
#شامل <stdio.h>

صحیح مین (صحیح argc, حرف** argv)
{
	اگر(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		باہر_نکلو(EXIT_FAILURE);
	}

	FILE* اندر = fopen(argv[1], "r");
	اگر(تہی == اندر)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		باہر_نکلو(EXIT_FAILURE);
	}

	حرف* جدول = "0123456789ABCDEF";
	صحیح c = fgetc(اندر);
	صحیح ستون = 40;
	جب(EOF != c)
	{
		fputc(جدول[c / 16], stdout);
		fputc(جدول[c % 16], stdout);
		ستون = ستون - 2;
		اگر(0 == ستون)
		{
			ستون = 40;
			fputc('\n', stdout);
		}
		ورنہ fputc(' ', stdout);
		c = fgetc(اندر);
	}
	fputc('\n', stdout);
	باہر_نکلو(EXIT_SUCCESS);
}
