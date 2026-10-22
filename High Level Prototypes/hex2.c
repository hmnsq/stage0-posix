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
	वर्ण* name;
};


/* Globals */
खाता* output;
संरचना entry* jump_table;
पूर्णाङ्क Base_Address;
पूर्णाङ्क ip;
वर्ण* scratch;

पूर्णाङ्क consume_token(खाता* source_file)
{
	पूर्णाङ्क i = 0;
	पूर्णाङ्क c = ख_अक्षर_लो(source_file);
	जबसम्म(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = ख_अक्षर_लो(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		यदि(खातापूर्ण == c) रोक;
	}

	फर्काऊ c;
}

पूर्णाङ्क Throwaway_token(खाता* source_file)
{
	पूर्णाङ्क c;
	गर
	{
		c = ख_अक्षर_लो(source_file);
		यदि(खातापूर्ण == c) रोक;
	} जबसम्म(!in_set(c, " \t\n>"));

	फर्काऊ c;
}

पूर्णाङ्क length(वर्ण* s)
{
	पूर्णाङ्क i = 0;
	जबसम्म(0 != s[i]) i = i + 1;
	फर्काऊ i;
}

शून्य Clear_Scratch(वर्ण* s)
{
	गर
	{
		s[0] = 0;
		s = s + 1;
	} जबसम्म(0 != s[0]);
}

शून्य Copy_String(वर्ण* a, वर्ण* b)
{
	जबसम्म(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

अचिन्हित GetTarget(वर्ण* c)
{
	संरचना entry* i;
	लागि(i = jump_table; रिक्त != i; i = i->next)
	{
		यदि(match(c, i->name))
		{
			फर्काऊ i->target;
		}
	}
	ख_माला_दो("Target label ", मानक_त्रुटि);
	ख_माला_दो(c, मानक_त्रुटि);
	ख_माला_दो(" is not valid\n", मानक_त्रुटि);
	निकास(निकास_त्रुटि);
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
	पूर्णाङ्क c = consume_token(source_file);
	entry->name = सुस्मृति(length(scratch) + 1, परिमाण(वर्ण));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	फर्काऊ c;
}

शून्य outputPointer(पूर्णाङ्क displacement, पूर्णाङ्क सङ्ख्या_of_bytes)
{
	अचिन्हित मान = displacement;

	जबसम्म(सङ्ख्या_of_bytes > 0)
	{
		अचिन्हित byte = मान % 256;
		मान = मान / 256;
		ख_अक्षर_दो(byte, output);
		सङ्ख्या_of_bytes = सङ्ख्या_of_bytes - 1;
	}
}

शून्य Update_Pointer(वर्ण ch)
{
	/* Calculate pointer size*/
	यदि(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	अन्यथा यदि(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	अन्यथा यदि('!' == ch) ip = ip + 1; /* Deal with ! */
	अन्यथा निकास(निकास_त्रुटि);
}

शून्य storePointer(वर्ण ch, खाता* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	पूर्णाङ्क base_sep_p = consume_token(source_file);

	/* Lookup token */
	पूर्णाङ्क target = GetTarget(scratch);
	पूर्णाङ्क displacement;

	पूर्णाङ्क base = ip;

	/* Change relative base address to :<base> */
	यदि ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	अन्यथा
	{
		displacement = base - target;
	}

	/* output calculated difference */
	यदि('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	अन्यथा यदि('$' == ch) outputPointer(target, 2); /* Deal with $ */
	अन्यथा यदि('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	अन्यथा यदि('&' == ch) outputPointer(target, 4); /* Deal with & */
	अन्यथा यदि('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	अन्यथा निकास(निकास_त्रुटि);
}

शून्य पङ्क्ति_Comment(खाता* source_file)
{
	पूर्णाङ्क c = ख_अक्षर_लो(source_file);
	जबसम्म(!in_set(c, "\n\r"))
	{
		यदि(खातापूर्ण == c) रोक;
		c = ख_अक्षर_लो(source_file);
	}
}

पूर्णाङ्क hex(पूर्णाङ्क c, खाता* source_file)
{
	यदि (in_set(c, "0123456789")) फर्काऊ (c - 48);
	अन्यथा यदि (in_set(c, "abcdef")) फर्काऊ (c - 87);
	अन्यथा यदि (in_set(c, "ABCDEF")) फर्काऊ (c - 55);
	अन्यथा यदि (in_set(c, "#;")) पङ्क्ति_Comment(source_file);
	फर्काऊ -1;
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

शून्य first_pass(खाता* आगत)
{
	toggle = FALSE;
	पूर्णाङ्क c;
	लागि(c = ख_अक्षर_लो(आगत); खातापूर्ण != c; c = ख_अक्षर_लो(आगत))
	{
		/* Check for and deal with label */
		यदि(':' == c)
		{
			c = storeLabel(आगत, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		यदि(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(आगत);
			यदि ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(आगत);
			}
		}
		अन्यथा process_byte(c, आगत, FALSE);
	}
}

शून्य second_pass(खाता* आगत)
{
	toggle = FALSE;
	hold = 0;

	पूर्णाङ्क c;
	लागि(c = ख_अक्षर_लो(आगत); खातापूर्ण != c; c = ख_अक्षर_लो(आगत))
	{
		यदि(':' == c) c = Throwaway_token(आगत); /* Deal with : */
		अन्यथा यदि(in_set(c, "!@$~%&")) storePointer(c, आगत);  /* Deal with !, @, $, ~, % and & */
		अन्यथा process_byte(c, आगत, TRUE);
	}
}

/* Standard C main program */
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
	jump_table = रिक्त;
	Base_Address = 0x8048000;
	output = मानक_निकास;
	scratch = सुस्मृति(max_string + 1, परिमाण(वर्ण));

	खाता* आगत = ख_खोलो(argv[1], "r");
	output = मानक_निकास;
	यदि(रिक्त != argv[2])
	{
		output = ख_खोलो(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(आगत);
	शुरुआत(आगत);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(आगत);

	/* Set file as executable */
	फर्काऊ निकास_सफल;
}
