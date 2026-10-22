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

#include <stdlib.h>
#include <stdio.h>

整数 メインページ (整数 argc, char** argv)
{
	もし(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		exit(EXIT_FAILURE);
	}

	FILE* in = fopen(argv[1], "r");
	もし(NULL == in)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		exit(EXIT_FAILURE);
	}

	char* table = "0123456789ABCDEF";
	整数 c = fgetc(in);
	整数 col = 40;
	ながら(EOF != c)
	{
		fputc(table[c / 16], stdout);
		fputc(table[c % 16], stdout);
		col = col - 2;
		もし(0 == col)
		{
			col = 40;
			fputc('\n', stdout);
		}
		否则 fputc(' ', stdout);
		c = fgetc(in);
	}
	fputc('\n', stdout);
	exit(EXIT_SUCCESS);
}
