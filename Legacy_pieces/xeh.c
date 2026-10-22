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

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>

పూర్ణాంకం ప్రధాన (పూర్ణాంకం argc, అక్షరం** argv)
{
	ఒకవేళ(argc != 2)
	{
		fputs("xeh $filename is the only valid input form\n", stderr);
		నిష్క్రమించు(EXIT_FAILURE);
	}

	FILE* లోపల = fopen(argv[1], "r");
	ఒకవేళ(ఖాళీ == లోపల)
	{
		fputs("Unable to open file: ", stderr);
		fputs(argv[1], stderr);
		fputs(" aborting to prevent problems\n", stderr);
		నిష్క్రమించు(EXIT_FAILURE);
	}

	అక్షరం* పట్టిక = "0123456789ABCDEF";
	పూర్ణాంకం c = fgetc(లోపల);
	పూర్ణాంకం నిలువు = 40;
	అయితే(EOF != c)
	{
		fputc(పట్టిక[c / 16], stdout);
		fputc(పట్టిక[c % 16], stdout);
		నిలువు = నిలువు - 2;
		ఒకవేళ(0 == నిలువు)
		{
			నిలువు = 40;
			fputc('\n', stdout);
		}
		లేకపోతే fputc(' ', stdout);
		c = fgetc(లోపల);
	}
	fputc('\n', stdout);
	నిష్క్రమించు(EXIT_SUCCESS);
}
