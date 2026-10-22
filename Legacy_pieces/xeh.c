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

정수 main (정수 argc, char** argv)
{
	만약(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		exit(EXIT_FAILURE);
	}

	FILE* in = fopen(argv[1], "r");
	만약(NULL == in)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		exit(EXIT_FAILURE);
	}

	char* table = "0123456789ABCDEF";
	정수 c = fgetc(in);
	정수 col = 40;
	동안(EOF != c)
	{
		fputc(table[c / 16], stdout);
		fputc(table[c % 16], stdout);
		col = col - 2;
		만약(0 == col)
		{
			col = 40;
			fputc('\n', stdout);
		}
		아니면 fputc(' ', stdout);
		c = fgetc(in);
	}
	fputc('\n', stdout);
	exit(EXIT_SUCCESS);
}
