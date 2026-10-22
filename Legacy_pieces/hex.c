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

#అన్తర్భావ<stdio.h>
#అన్తర్భావ<stdlib.h>
#అన్తర్భావ<stdbool.h>

శూన్య purge_పంక్తి_comments()
{
	పూర్ణాంకం c;
	చేయి
	{
		c = getchar();

		ఒకవేళ (EOF == c)
		{
			నిష్క్రమించు(EXIT_SUCCESS);
		}
	} అయితే ( '\n' != c );
}

పూర్ణాంకం hex(పూర్ణాంకం c)
{
	ఒకవేళ (c >= '0' && c <= '9')
	{
		తిరిగి (c - 48);
	}
	లేకపోతే ఒకవేళ (c >= 'a' && c <= 'z')
	{
		తిరిగి (c - 87);
	}
	లేకపోతే ఒకవేళ (c >= 'A' && c <= 'Z')
	{
		తిరిగి (c - 55);
	}

	రాయి("You managed to call a hex function without a hex value!!!\n");
	నిష్క్రమించు(EXIT_FAILURE);
}

పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
	పూర్ణాంకం c;
	పూర్ణాంకం కూడిక = 0;
	bool toggle;
	toggle = false;

	చేయి
	{
		c = getchar();
		ఒకవేళ ((c == '#') || (c == ';')) purge_పంక్తి_comments();
		లేకపోతే ఒకవేళ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			ఒకవేళ(!toggle)
			{
				కూడిక = hex(c);
				toggle = true;
			}
			లేకపోతే
			{
				కూడిక = (కూడిక << 4) + hex(c);
				toggle = false;
				fputc(కూడిక, stdout);
			}
		}
	}అయితే(c != EOF);

	నిష్క్రమించు(EXIT_SUCCESS);
}
