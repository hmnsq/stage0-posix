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

فراغ purge_line_comments()
{
	صحيح c;
	ܥܒܕ
	{
		c = getchar();

		ܐܢ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ܟܡܐ ( '\n' != c );
}

صحيح hex(صحيح c)
{
	ܐܢ (c >= '0' && c <= '9')
	{
		ܦܢܐ (c - 48);
	}
	ܐܚܪܢܐ ܐܢ (c >= 'a' && c <= 'z')
	{
		ܦܢܐ (c - 87);
	}
	ܐܚܪܢܐ ܐܢ (c >= 'A' && c <= 'Z')
	{
		ܦܢܐ (c - 55);
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

	ܥܒܕ
	{
		c = getchar();
		ܐܢ ((c == '#') || (c == ';')) purge_line_comments();
		ܐܚܪܢܐ ܐܢ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ܐܢ(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ܐܚܪܢܐ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ܟܡܐ(c != EOF);

	exit(EXIT_SUCCESS);
}
