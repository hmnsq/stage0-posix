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

#define max_string 4096

संरचना entry
{
	संरचना entry* next;
	अचिन्हित target;
	वर्ण name;
};


/* Globals */
खाता* output;
संरचना entry* jump_table;
पूर्णाङ्क ip;

अचिन्हित GetTarget(वर्ण c)
{
	संरचना entry* i;
	प्रति(i = jump_table; रिक्त != i; i = i->next)
	{
		यदि(c == i->name)
		{
			प्रत्यागम i->target;
		}
	}
	प्रत्यागम 0;
}

पूर्णाङ्क storeLabel(खाता* source_file, पूर्णाङ्क ip)
{
	संरचना entry* entry = सुस्मृति(1, परिमाण(संरचना entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = ख_अक्षर_लो(source_file);
	प्रत्यागम -1;
}


शून्य outputPointer(पूर्णाङ्क displacement, पूर्णाङ्क संख्या_of_bytes)
{
	अचिन्हित मूल्य = displacement;

	यावत्(संख्या_of_bytes > 0)
	{
		अचिन्हित byte = मूल्य % 256;
		मूल्य = मूल्य / 256;
		ख_अक्षर_दो(byte, output);
		संख्या_of_bytes = संख्या_of_bytes - 1;
	}
}

शून्य Update_Pointer(वर्ण ch)
{
	/* Calculate pointer size*/
	यदि(in_set(ch, "%")) ip = ip + 4;
	अन्यथा निकास(निकास_त्रुटि);
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
	अन्यथा निकास(निकास_त्रुटि);
}

शून्य पंक्ति_Comment(खाता* source_file)
{
	पूर्णाङ्क c = ख_अक्षर_लो(source_file);
	यावत्(!in_set(c, "\n\r"))
	{
		यदि(खातापूर्ण == c) भङ्ग;
		c = ख_अक्षर_लो(source_file);
	}
}

पूर्णाङ्क hex(पूर्णाङ्क c, खाता* source_file)
{
	यदि (in_set(c, "0123456789")) प्रत्यागम (c - 48);
	अन्यथा यदि (in_set(c, "abcdef")) प्रत्यागम (c - 87);
	अन्यथा यदि (in_set(c, "ABCDEF")) प्रत्यागम (c - 55);
	अन्यथा यदि (in_set(c, "#;")) पंक्ति_Comment(source_file);
	प्रत्यागम -1;
}

पूर्णाङ्क hold;
पूर्णाङ्क toggle;
शून्य process_byte(वर्ण c, खाता* source_file, पूर्णाङ्क लिख)
{
	यदि(0 <= hex(c, source_file))
	{
		यदि(toggle)
		{
			यदि(लिख) ख_अक्षर_दो(((hold * 16)) + hex(c, source_file), output);
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

शून्य first_pass(खाता* निवेश)
{
	toggle = FALSE;
	पूर्णाङ्क c;
	प्रति(c = ख_अक्षर_लो(निवेश); खातापूर्ण != c; c = ख_अक्षर_लो(निवेश))
	{
		/* Check for and deal with label */
		यदि(':' == c)
		{
			c = storeLabel(निवेश, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		यदि(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = ख_अक्षर_लो(निवेश);
		}
		अन्यथा process_byte(c, निवेश, FALSE);
	}
}

शून्य second_pass(खाता* निवेश)
{
	toggle = FALSE;
	hold = 0;

	पूर्णाङ्क c;
	प्रति(c = ख_अक्षर_लो(निवेश); खातापूर्ण != c; c = ख_अक्षर_लो(निवेश))
	{
		यदि(':' == c) c = ख_अक्षर_लो(निवेश); /* Deal with : */
		अन्यथा यदि(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		अन्यथा process_byte(c, निवेश, TRUE);
	}
}

/* Standard C main program */
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
	jump_table = रिक्त;
	output = मानक_निकास;

	खाता* निवेश = ख_खोलो(argv[1], "r");
	output = मानक_निकास;
	यदि(रिक्त != argv[2])
	{
		output = ख_खोलो(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(निवेश);
	शुरुआत(निवेश);

	/* Fix all the references*/
	ip = 0;
	second_pass(निवेश);

	/* Set file as executable */
	प्रत्यागम निकास_सफल;
}
