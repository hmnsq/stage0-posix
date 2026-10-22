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

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव "../M2libc/bootstrappable.h"

#परिभाषय max_string 4096

संरचना प्रविष्टिः
{
	संरचना प्रविष्टिः* अग्र;
	अचिह्नित target;
	वर्ण name;
};


/* Globals */
FILE* output;
संरचना प्रविष्टिः* jump_सारणी;
पूर्णाङ्क ip;

अचिह्नित GetTarget(वर्ण c)
{
	संरचना प्रविष्टिः* i;
	प्रति(i = jump_सारणी; रिक्त != i; i = i->अग्र)
	{
		यदि(c == i->name)
		{
			प्रत्यागम i->target;
		}
	}
	प्रत्यागम 0;
}

पूर्णाङ्क storeLabel(FILE* source_file, पूर्णाङ्क ip)
{
	संरचना प्रविष्टिः* प्रविष्टिः = शून्यस्मृतिं_देहि(1, परिमाण(संरचना प्रविष्टिः));

	/* Ensure we have target address */
	प्रविष्टिः->target = ip;

	/* Prepend to list */
	प्रविष्टिः->अग्र = jump_सारणी;
	jump_सारणी = प्रविष्टिः;

	/* Store string */
	प्रविष्टिः->name = fgetc(source_file);
	प्रत्यागम -1;
}


शून्य outputPointer(पूर्णाङ्क displacement, पूर्णाङ्क सङ्ख्या_सम्बन्धि_अष्टकानि)
{
	अचिह्नित मानम् = displacement;

	यावत्(सङ्ख्या_सम्बन्धि_अष्टकानि > 0)
	{
		अचिह्नित byte = मानम् % 256;
		मानम् = मानम् / 256;
		fputc(byte, output);
		सङ्ख्या_सम्बन्धि_अष्टकानि = सङ्ख्या_सम्बन्धि_अष्टकानि - 1;
	}
}

शून्य Update_Pointer(वर्ण ch)
{
	/* Calculate pointer size*/
	यदि(अन्तर्_समुच्चयः(ch, "%")) ip = ip + 4;
	अन्यथा निर्गच्छ(EXIT_FAILURE);
}

शून्य storePointer(वर्ण ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	पूर्णाङ्क target = GetTarget(ch);
	पूर्णाङ्क displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	यदि('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	अन्यथा निर्गच्छ(EXIT_FAILURE);
}

शून्य पङ्क्तिः_Comment(FILE* source_file)
{
	पूर्णाङ्क c = fgetc(source_file);
	यावत्(!अन्तर्_समुच्चयः(c, "\n\r"))
	{
		यदि(EOF == c) भङ्ग;
		c = fgetc(source_file);
	}
}

पूर्णाङ्क hex(पूर्णाङ्क c, FILE* source_file)
{
	यदि (अन्तर्_समुच्चयः(c, "0123456789")) प्रत्यागम (c - 48);
	अन्यथा यदि (अन्तर्_समुच्चयः(c, "abcdef")) प्रत्यागम (c - 87);
	अन्यथा यदि (अन्तर्_समुच्चयः(c, "ABCDEF")) प्रत्यागम (c - 55);
	अन्यथा यदि (अन्तर्_समुच्चयः(c, "#;")) पङ्क्तिः_Comment(source_file);
	प्रत्यागम -1;
}

पूर्णाङ्क hold;
पूर्णाङ्क toggle;
शून्य process_byte(वर्ण c, FILE* source_file, पूर्णाङ्क लिख)
{
	यदि(0 <= hex(c, source_file))
	{
		यदि(toggle)
		{
			यदि(लिख) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		अन्यथा
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

शून्य first_pass(FILE* निवेशः)
{
	toggle = FALSE;
	पूर्णाङ्क c;
	प्रति(c = fgetc(निवेशः); EOF != c; c = fgetc(निवेशः))
	{
		/* Check for and deal with label */
		यदि(':' == c)
		{
			c = storeLabel(निवेशः, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		यदि(अन्तर्_समुच्चयः(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(निवेशः);
		}
		अन्यथा process_byte(c, निवेशः, FALSE);
	}
}

शून्य second_pass(FILE* निवेशः)
{
	toggle = FALSE;
	hold = 0;

	पूर्णाङ्क c;
	प्रति(c = fgetc(निवेशः); EOF != c; c = fgetc(निवेशः))
	{
		यदि(':' == c) c = fgetc(निवेशः); /* Deal with : */
		अन्यथा यदि(अन्तर्_समुच्चयः(c, "%")) storePointer(c);  /* Deal with %*/
		अन्यथा process_byte(c, निवेशः, TRUE);
	}
}

/* Standard C main program */
पूर्णाङ्क मुख्यम्(पूर्णाङ्क argc, वर्ण **argv)
{
	jump_सारणी = रिक्त;
	output = stdout;

	FILE* निवेशः = fopen(argv[1], "r");
	output = stdout;
	यदि(रिक्त != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(निवेशः);
	rewind(निवेशः);

	/* Fix all the references*/
	ip = 0;
	second_pass(निवेशः);

	/* Set file as executable */
	प्रत्यागम EXIT_SUCCESS;
}
