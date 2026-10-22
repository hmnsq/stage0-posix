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

#અન્તર્ભાવ<stdio.h>
#અન્તર્ભાવ<stdlib.h>
#અન્તર્ભાવ<stdbool.h>

શૂન્ય purge_લીટી_comments()
{
	પૂર્ણાંક c;
	કરો
	{
		c = getchar();

		જો (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} જ્યારે ( '\n' != c );
}

પૂર્ણાંક hex(પૂર્ણાંક c)
{
	જો (c >= '0' && c <= '9')
	{
		પાછા (c - 48);
	}
	નહીંતર જો (c >= 'a' && c <= 'z')
	{
		પાછા (c - 87);
	}
	નહીંતર જો (c >= 'A' && c <= 'Z')
	{
		પાછા (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
	પૂર્ણાંક c;
	પૂર્ણાંક sum = 0;
	bool toggle;
	toggle = false;

	કરો
	{
		c = getchar();
		જો ((c == '#') || (c == ';')) purge_લીટી_comments();
		નહીંતર જો ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			જો(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			નહીંતર
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}જ્યારે(c != EOF);

	exit(EXIT_SUCCESS);
}
