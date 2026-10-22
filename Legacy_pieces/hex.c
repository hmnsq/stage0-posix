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

#அந்தர்பாவ<stdio.h>
#அந்தர்பாவ<stdlib.h>
#அந்தர்பாவ<stdbool.h>

வெறுமை purge_வரி_comments()
{
	முழு c;
	செய்
	{
		c = getchar();

		எனில் (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} போது ( '\n' != c );
}

முழு hex(முழு c)
{
	எனில் (c >= '0' && c <= '9')
	{
		திரும்பு (c - 48);
	}
	இல்லையெனில் எனில் (c >= 'a' && c <= 'z')
	{
		திரும்பு (c - 87);
	}
	இல்லையெனில் எனில் (c >= 'A' && c <= 'Z')
	{
		திரும்பு (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

முழு முதன்மை(முழு argc, எழுத்து *argv[])
{
	முழு c;
	முழு sum = 0;
	bool toggle;
	toggle = false;

	செய்
	{
		c = getchar();
		எனில் ((c == '#') || (c == ';')) purge_வரி_comments();
		இல்லையெனில் எனில் ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			எனில்(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			இல்லையெனில்
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}போது(c != EOF);

	exit(EXIT_SUCCESS);
}
