/* Copyright (C) 2020 Jeremiah Orians
 * Copyright (C) 2017 Jan Nieuwenhuizen <janneke@gnu.org>
 * This file is part of stage0
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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ "../M2libc/bootstrappable.h"

#నిర్వచించు max_string 4096

నిర్మాణం నమోదు_2
{
	నిర్మాణం నమోదు_2* తదుపరి;
	గుర్తులేని లక్ష్యం;
	అక్షరం name;
};


/* Globals */
FILE* output;
నిర్మాణం నమోదు_2* jump_పట్టిక;
పూర్ణాంకం ip;

గుర్తులేని Get_లక్ష్యం(అక్షరం c)
{
	నిర్మాణం నమోదు_2* i;
	కొరకు(i = jump_పట్టిక; ఖాళీ != i; i = i->తదుపరి)
	{
		ఒకవేళ(c == i->name)
		{
			తిరిగి i->లక్ష్యం;
		}
	}
	తిరిగి 0;
}

పూర్ణాంకం storeLabel(FILE* ఆధారం_file, పూర్ణాంకం ip)
{
	నిర్మాణం నమోదు_2* నమోదు_2 = సున్నా_జ్ఞాపకం_ఇవ్వు(1, పరిమాణం(నిర్మాణం నమోదు_2));

	/* Ensure we have target address */
	నమోదు_2->లక్ష్యం = ip;

	/* Prepend to list */
	నమోదు_2->తదుపరి = jump_పట్టిక;
	jump_పట్టిక = నమోదు_2;

	/* Store string */
	నమోదు_2->name = fgetc(ఆధారం_file);
	తిరిగి -1;
}


శూన్య output_సూచిక(పూర్ణాంకం displacement, పూర్ణాంకం సంఖ్య_యొక్క_బైట్లు)
{
	గుర్తులేని విలువ = displacement;

	అయితే(సంఖ్య_యొక్క_బైట్లు > 0)
	{
		గుర్తులేని byte = విలువ % 256;
		విలువ = విలువ / 256;
		fputc(byte, output);
		సంఖ్య_యొక్క_బైట్లు = సంఖ్య_యొక్క_బైట్లు - 1;
	}
}

శూన్య Update_సూచిక(అక్షరం ch)
{
	/* Calculate pointer size*/
	ఒకవేళ(లోపల_సమితి(ch, "%")) ip = ip + 4;
	లేకపోతే నిష్క్రమించు(EXIT_FAILURE);
}

శూన్య store_సూచిక(అక్షరం ch)
{
	/* Get string of pointer */
	Update_సూచిక(ch);

	/* Lookup token */
	పూర్ణాంకం లక్ష్యం = Get_లక్ష్యం(ch);
	పూర్ణాంకం displacement;

	/* Change relative base address to :<base> */
	displacement = (లక్ష్యం - ip);

	/* output calculated difference */
	ఒకవేళ('%' == ch) output_సూచిక(displacement, 4);  /* Deal with % */
	లేకపోతే నిష్క్రమించు(EXIT_FAILURE);
}

శూన్య పంక్తి_Comment(FILE* ఆధారం_file)
{
	పూర్ణాంకం c = fgetc(ఆధారం_file);
	అయితే(!లోపల_సమితి(c, "\n\r"))
	{
		ఒకవేళ(EOF == c) ఆపు;
		c = fgetc(ఆధారం_file);
	}
}

పూర్ణాంకం hex(పూర్ణాంకం c, FILE* ఆధారం_file)
{
	ఒకవేళ (లోపల_సమితి(c, "0123456789")) తిరిగి (c - 48);
	లేకపోతే ఒకవేళ (లోపల_సమితి(c, "abcdef")) తిరిగి (c - 87);
	లేకపోతే ఒకవేళ (లోపల_సమితి(c, "ABCDEF")) తిరిగి (c - 55);
	లేకపోతే ఒకవేళ (లోపల_సమితి(c, "#;")) పంక్తి_Comment(ఆధారం_file);
	తిరిగి -1;
}

పూర్ణాంకం hold;
పూర్ణాంకం toggle;
శూన్య process_byte(అక్షరం c, FILE* ఆధారం_file, పూర్ణాంకం లిఖ)
{
	ఒకవేళ(0 <= hex(c, ఆధారం_file))
	{
		ఒకవేళ(toggle)
		{
			ఒకవేళ(లిఖ) fputc(((hold * 16)) + hex(c, ఆధారం_file), output);
			ip = ip + 1;
			hold = 0;
		}
		లేకపోతే
		{
			hold = hex(c, ఆధారం_file);
		}
		toggle = !toggle;
	}
}

శూన్య మొదటి_pass(FILE* ఆదానం)
{
	toggle = FALSE;
	పూర్ణాంకం c;
	కొరకు(c = fgetc(ఆదానం); EOF != c; c = fgetc(ఆదానం))
	{
		/* Check for and deal with label */
		ఒకవేళ(':' == c)
		{
			c = storeLabel(ఆదానం, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ఒకవేళ(లోపల_సమితి(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_సూచిక(c);
			c = fgetc(ఆదానం);
		}
		లేకపోతే process_byte(c, ఆదానం, FALSE);
	}
}

శూన్య రెండవ_pass(FILE* ఆదానం)
{
	toggle = FALSE;
	hold = 0;

	పూర్ణాంకం c;
	కొరకు(c = fgetc(ఆదానం); EOF != c; c = fgetc(ఆదానం))
	{
		ఒకవేళ(':' == c) c = fgetc(ఆదానం); /* Deal with : */
		లేకపోతే ఒకవేళ(లోపల_సమితి(c, "%")) store_సూచిక(c);  /* Deal with %*/
		లేకపోతే process_byte(c, ఆదానం, TRUE);
	}
}

/* Standard C main program */
పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
	jump_పట్టిక = ఖాళీ;
	output = stdout;

	FILE* ఆదానం = fopen(argv[1], "r");
	output = stdout;
	ఒకవేళ(ఖాళీ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	మొదటి_pass(ఆదానం);
	rewind(ఆదానం);

	/* Fix all the references*/
	ip = 0;
	రెండవ_pass(ఆదానం);

	/* Set file as executable */
	తిరిగి EXIT_SUCCESS;
}
