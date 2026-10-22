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

#ଅନ୍ତର୍ଭାଵ<stdio.h>
#ଅନ୍ତର୍ଭାଵ<stdlib.h>
#ଅନ୍ତର୍ଭାଵ<stdbool.h>

ଶୂନ୍ୟ purge_ଧାଡ଼ି_comments()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	କର
	{
		c = getchar();

		ଯଦି (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ଯେତେବେଳେ ( '\n' != c );
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hex(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
	ଯଦି (c >= '0' && c <= '9')
	{
		ଫେରନ୍ତୁ (c - 48);
	}
	ଅନ୍ୟଥା ଯଦି (c >= 'a' && c <= 'z')
	{
		ଫେରନ୍ତୁ (c - 87);
	}
	ଅନ୍ୟଥା ଯଦି (c >= 'A' && c <= 'Z')
	{
		ଫେରନ୍ତୁ (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sum = 0;
	bool toggle;
	toggle = false;

	କର
	{
		c = getchar();
		ଯଦି ((c == '#') || (c == ';')) purge_ଧାଡ଼ି_comments();
		ଅନ୍ୟଥା ଯଦି ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ଯଦି(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ଅନ୍ୟଥା
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ଯେତେବେଳେ(c != EOF);

	exit(EXIT_SUCCESS);
}
