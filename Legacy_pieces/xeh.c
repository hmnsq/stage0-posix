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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

ਪੂਰਨ ਮੁੱਖ (ਪੂਰਨ argc, ਅੱਖਰ** argv)
{
	ਜੇ(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	}

	FILE* ਵਿੱਚ_2 = fopen(argv[1], "r");
	ਜੇ(ਨਿਰਾ == ਵਿੱਚ_2)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	}

	ਅੱਖਰ* ਸਾਰਣੀ = "0123456789ABCDEF";
	ਪੂਰਨ c = fgetc(ਵਿੱਚ_2);
	ਪੂਰਨ ਖਾਨਾ = 40;
	ਜਦੋਂ(EOF != c)
	{
		fputc(ਸਾਰਣੀ[c / 16], stdout);
		fputc(ਸਾਰਣੀ[c % 16], stdout);
		ਖਾਨਾ = ਖਾਨਾ - 2;
		ਜੇ(0 == ਖਾਨਾ)
		{
			ਖਾਨਾ = 40;
			fputc('\n', stdout);
		}
		ਨਹੀਂਤਾਂ fputc(' ', stdout);
		c = fgetc(ਵਿੱਚ_2);
	}
	fputc('\n', stdout);
	ਬਾਹਰ_ਨਿਕਲੋ(EXIT_SUCCESS);
}
