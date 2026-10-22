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
	ຈຳນວນເຕັມ c;
	do
	{
		c = getchar();

		ຖ້າ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ໃນຂະນະທີ່ ( '\n' != c );
}

ຈຳນວນເຕັມ hex(ຈຳນວນເຕັມ c)
{
	ຖ້າ (c >= '0' && c <= '9')
	{
		ສົ່ງຄືນ (c - 48);
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c >= 'a' && c <= 'z')
	{
		ສົ່ງຄືນ (c - 87);
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (c >= 'A' && c <= 'Z')
	{
		ສົ່ງຄືນ (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
	ຈຳນວນເຕັມ c;
	ຈຳນວນເຕັມ sum = 0;
	bool toggle;
	toggle = false;

	do
	{
		c = getchar();
		ຖ້າ ((c == '#') || (c == ';')) purge_line_comments();
		ບໍ່ດັ່ງນັ້ນ ຖ້າ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ຖ້າ(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			ບໍ່ດັ່ງນັ້ນ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ໃນຂະນະທີ່(c != EOF);

	exit(EXIT_SUCCESS);
}
