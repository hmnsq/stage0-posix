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
	वर्ण* name;
};


/* Globals */
FILE* output;
संरचना प्रविष्टिः* jump_सारणी;
पूर्णाङ्क आधारः_स्थानाङ्कः;
पूर्णाङ्क ip;
वर्ण* scratch;

पूर्णाङ्क consume_token(FILE* source_file)
{
	पूर्णाङ्क i = 0;
	पूर्णाङ्क c = fgetc(source_file);
	यावत्(!अन्तर्_समुच्चयः(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		यदि(EOF == c) भङ्ग;
	}

	प्रत्यागम c;
}

पूर्णाङ्क Throwaway_token(FILE* source_file)
{
	पूर्णाङ्क c;
	कुरु
	{
		c = fgetc(source_file);
		यदि(EOF == c) भङ्ग;
	} यावत्(!अन्तर्_समुच्चयः(c, " \t\n>"));

	प्रत्यागम c;
}

पूर्णाङ्क दीर्घता(वर्ण* s)
{
	पूर्णाङ्क i = 0;
	यावत्(0 != s[i]) i = i + 1;
	प्रत्यागम i;
}

शून्य Clear_Scratch(वर्ण* s)
{
	कुरु
	{
		s[0] = 0;
		s = s + 1;
	} यावत्(0 != s[0]);
}

शून्य Copy_String(वर्ण* a, वर्ण* b)
{
	यावत्(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

अचिह्नित GetTarget(वर्ण* c)
{
	संरचना प्रविष्टिः* i;
	प्रति(i = jump_सारणी; रिक्त != i; i = i->अग्र)
	{
		यदि(मेलनम्(c, i->name))
		{
			प्रत्यागम i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	निर्गच्छ(EXIT_FAILURE);
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
	पूर्णाङ्क c = consume_token(source_file);
	प्रविष्टिः->name = शून्यस्मृतिं_देहि(दीर्घता(scratch) + 1, परिमाण(वर्ण));
	Copy_String(scratch, प्रविष्टिः->name);
	Clear_Scratch(scratch);

	प्रत्यागम c;
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
	यदि(अन्तर्_समुच्चयः(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	अन्यथा यदि(अन्तर्_समुच्चयः(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	अन्यथा यदि('!' == ch) ip = ip + 1; /* Deal with ! */
	अन्यथा निर्गच्छ(EXIT_FAILURE);
}

शून्य storePointer(वर्ण ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	पूर्णाङ्क आधारः_sep_p = consume_token(source_file);

	/* Lookup token */
	पूर्णाङ्क target = GetTarget(scratch);
	पूर्णाङ्क displacement;

	पूर्णाङ्क आधारः = ip;

	/* Change relative base address to :<base> */
	यदि ('>' == आधारः_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		आधारः = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - आधारः);
	}
	अन्यथा
	{
		displacement = आधारः - target;
	}

	/* output calculated difference */
	यदि('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	अन्यथा यदि('$' == ch) outputPointer(target, 2); /* Deal with $ */
	अन्यथा यदि('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	अन्यथा यदि('&' == ch) outputPointer(target, 4); /* Deal with & */
	अन्यथा यदि('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
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
		यदि(अन्तर्_समुच्चयः(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(निवेशः);
			यदि ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(निवेशः);
			}
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
		यदि(':' == c) c = Throwaway_token(निवेशः); /* Deal with : */
		अन्यथा यदि(अन्तर्_समुच्चयः(c, "!@$~%&")) storePointer(c, निवेशः);  /* Deal with !, @, $, ~, % and & */
		अन्यथा process_byte(c, निवेशः, TRUE);
	}
}

/* Standard C main program */
पूर्णाङ्क मुख्यम्(पूर्णाङ्क argc, वर्ण **argv)
{
	jump_सारणी = रिक्त;
	आधारः_स्थानाङ्कः = 0x8048000;
	output = stdout;
	scratch = शून्यस्मृतिं_देहि(max_string + 1, परिमाण(वर्ण));

	FILE* निवेशः = fopen(argv[1], "r");
	output = stdout;
	यदि(रिक्त != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = आधारः_स्थानाङ्कः;
	first_pass(निवेशः);
	rewind(निवेशः);

	/* Fix all the references*/
	ip = आधारः_स्थानाङ्कः;
	second_pass(निवेशः);

	/* Set file as executable */
	प्रत्यागम EXIT_SUCCESS;
}
