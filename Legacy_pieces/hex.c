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

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

void purge_line_comments()
{
	puur_naa_nka c;
	do
	{
		c = getchar();

		ပေႃး (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ၶၢဝ်း ( '\n' != c );
}

puur_naa_nka hex(puur_naa_nka c)
{
	ပေႃး (c >= '0' && c <= '9')
	{
		ပွၵ်ႈ (c - 48);
	}
	ပေႃးဢမ်ႇ ပေႃး (c >= 'a' && c <= 'z')
	{
		ပွၵ်ႈ (c - 87);
	}
	ပေႃးဢမ်ႇ ပေႃး (c >= 'A' && c <= 'Z')
	{
		ပွၵ်ႈ (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

puur_naa_nka main(puur_naa_nka argc, var_na *argv[])
{
	puur_naa_nka c;
	puur_naa_nka sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		ပေႃး ((c == '#') || (c == ';')) purge_line_comments();
		ပေႃးဢမ်ႇ ပေႃး ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ပေႃး(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ပေႃးဢမ်ႇ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ၶၢဝ်း(c != EOF);

	exit(EXIT_SUCCESS);
}
