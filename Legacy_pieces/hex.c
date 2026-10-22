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

#ਅਨ੍ਤਰ੍ਭਾਵ<stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ<stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ<stdbool.h>

ਖਾਲੀ purge_ਸਤਰ_comments()
{
	ਪੂਰਨ c;
	ਕਰੋ
	{
		c = getchar();

		ਜੇ (EOF == c)
		{
			ਬਾਹਰ_ਨਿਕਲੋ(EXIT_SUCCESS);
		}
	} ਜਦੋਂ ( '\n' != c );
}

ਪੂਰਨ hex(ਪੂਰਨ c)
{
	ਜੇ (c >= '0' && c <= '9')
	{
		ਵਾਪਸ (c - 48);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c >= 'a' && c <= 'z')
	{
		ਵਾਪਸ (c - 87);
	}
	ਨਹੀਂਤਾਂ ਜੇ (c >= 'A' && c <= 'Z')
	{
		ਵਾਪਸ (c - 55);
	}

	ਲਿਖੋ("You managed to call a hex function without a hex value!!!\n");
	ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
}

ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
	ਪੂਰਨ c;
	ਪੂਰਨ ਜੋੜ = 0;
	bool toggle;
	toggle = false;

	ਕਰੋ
	{
		c = getchar();
		ਜੇ ((c == '#') || (c == ';')) purge_ਸਤਰ_comments();
		ਨਹੀਂਤਾਂ ਜੇ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ਜੇ(!toggle)
			{
				ਜੋੜ = hex(c);
				toggle = true;
			}
			ਨਹੀਂਤਾਂ
			{
				ਜੋੜ = (ਜੋੜ << 4) + hex(c);
				toggle = false;
				fputc(ਜੋੜ, stdout);
			}
		}
	}ਜਦੋਂ(c != EOF);

	ਬਾਹਰ_ਨਿਕਲੋ(EXIT_SUCCESS);
}
