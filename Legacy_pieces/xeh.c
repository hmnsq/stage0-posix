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

#ضمن <stdlib.h>
#ضمن <stdio.h>

صحيح مکيه (صحيح argc, حرف** argv)
{
	إذا(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		ٻاهر_نڪرو(EXIT_FAILURE);
	}

	FILE* اندريون = fopen(argv[1], "r");
	إذا(عدم == اندريون)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		ٻاهر_نڪرو(EXIT_FAILURE);
	}

	حرف* جدول = "0123456789ABCDEF";
	صحيح c = fgetc(اندريون);
	صحيح کانو = 40;
	طالما(EOF != c)
	{
		fputc(جدول[c / 16], stdout);
		fputc(جدول[c % 16], stdout);
		کانو = کانو - 2;
		إذا(0 == کانو)
		{
			کانو = 40;
			fputc('\n', stdout);
		}
		وإلا fputc(' ', stdout);
		c = fgetc(اندريون);
	}
	fputc('\n', stdout);
	ٻاهر_نڪرو(EXIT_SUCCESS);
}
