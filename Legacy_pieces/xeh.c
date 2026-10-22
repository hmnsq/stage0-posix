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

صحيح اصلي (صحيح argc, حرف** argv)
{
	إذا(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		وتل(EXIT_FAILURE);
	}

	FILE* دننه = fopen(argv[1], "r");
	إذا(عدم == دننه)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		وتل(EXIT_FAILURE);
	}

	حرف* جدول = "0123456789ABCDEF";
	صحيح c = fgetc(دننه);
	صحيح ستون = 40;
	طالما(EOF != c)
	{
		fputc(جدول[c / 16], stdout);
		fputc(جدول[c % 16], stdout);
		ستون = ستون - 2;
		إذا(0 == ستون)
		{
			ستون = 40;
			fputc('\n', stdout);
		}
		وإلا fputc(' ', stdout);
		c = fgetc(دننه);
	}
	fputc('\n', stdout);
	وتل(EXIT_SUCCESS);
}
