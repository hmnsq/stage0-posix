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

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>

पूर्णाङ्क main (पूर्णाङ्क argc, वर्ण** argv)
{
	जइ(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		exit(EXIT_FAILURE);
	}

	FILE* in = fopen(argv[1], "r");
	जइ(रिक्त == in)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		exit(EXIT_FAILURE);
	}

	वर्ण* table = "0123456789ABCDEF";
	पूर्णाङ्क c = fgetc(in);
	पूर्णाङ्क col = 40;
	जाव(EOF != c)
	{
		fputc(table[c / 16], stdout);
		fputc(table[c % 16], stdout);
		col = col - 2;
		जइ(0 == col)
		{
			col = 40;
			fputc('\n', stdout);
		}
		अण्णहा fputc(' ', stdout);
		c = fgetc(in);
	}
	fputc('\n', stdout);
	exit(EXIT_SUCCESS);
}
