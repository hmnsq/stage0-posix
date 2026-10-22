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
	अक्षर* name;
};


/* Globals */
खाता* output;
संरचना entry* jump_table;
पूर्णांक Base_Address;
पूर्णांक ip;
अक्षर* scratch;

पूर्णांक consume_token(खाता* source_file)
{
	पूर्णांक i = 0;
	पूर्णांक c = ख_अक्षर_लो(source_file);
	पर्यंत(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = ख_अक्षर_लो(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		जर(खातापूर्ण == c) थांबा;
	}

	परत c;
}

पूर्णांक Throwaway_token(खाता* source_file)
{
	पूर्णांक c;
	करा
	{
		c = ख_अक्षर_लो(source_file);
		जर(खातापूर्ण == c) थांबा;
	} पर्यंत(!in_set(c, " \t\n>"));

	परत c;
}

पूर्णांक length(अक्षर* s)
{
	पूर्णांक i = 0;
	पर्यंत(0 != s[i]) i = i + 1;
	परत i;
}

शून्य Clear_Scratch(अक्षर* s)
{
	करा
	{
		s[0] = 0;
		s = s + 1;
	} पर्यंत(0 != s[0]);
}

शून्य Copy_String(अक्षर* a, अक्षर* b)
{
	पर्यंत(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

अचिन्हित GetTarget(अक्षर* c)
{
	संरचना entry* i;
	साठी(i = jump_table; रिक्त != i; i = i->next)
	{
		जर(match(c, i->name))
		{
			परत i->target;
		}
	}
	ख_माला_दो("Target label ", मानक_त्रुटि);
	ख_माला_दो(c, मानक_त्रुटि);
	ख_माला_दो(" is not valid\n", मानक_त्रुटि);
	निकास(निकास_त्रुटि);
}

पूर्णांक storeLabel(खाता* source_file, पूर्णांक ip)
{
	संरचना entry* entry = सुस्मृति(1, आकार(संरचना entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	पूर्णांक c = consume_token(source_file);
	entry->name = सुस्मृति(length(scratch) + 1, आकार(अक्षर));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	परत c;
}

शून्य outputPointer(पूर्णांक displacement, पूर्णांक संख्या_of_bytes)
{
	अचिन्हित मूल्य = displacement;

	पर्यंत(संख्या_of_bytes > 0)
	{
		अचिन्हित byte = मूल्य % 256;
		मूल्य = मूल्य / 256;
		ख_अक्षर_दो(byte, output);
		संख्या_of_bytes = संख्या_of_bytes - 1;
	}
}

शून्य Update_Pointer(अक्षर ch)
{
	/* Calculate pointer size*/
	जर(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	अन्यथा जर(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	अन्यथा जर('!' == ch) ip = ip + 1; /* Deal with ! */
	अन्यथा निकास(निकास_त्रुटि);
}

शून्य storePointer(अक्षर ch, खाता* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	पूर्णांक base_sep_p = consume_token(source_file);

	/* Lookup token */
	पूर्णांक target = GetTarget(scratch);
	पूर्णांक displacement;

	पूर्णांक base = ip;

	/* Change relative base address to :<base> */
	जर ('>' == base_sep_p)
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
	जर('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	अन्यथा जर('$' == ch) outputPointer(target, 2); /* Deal with $ */
	अन्यथा जर('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	अन्यथा जर('&' == ch) outputPointer(target, 4); /* Deal with & */
	अन्यथा जर('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	अन्यथा निकास(निकास_त्रुटि);
}

शून्य ओळ_Comment(खाता* source_file)
{
	पूर्णांक c = ख_अक्षर_लो(source_file);
	पर्यंत(!in_set(c, "\n\r"))
	{
		जर(खातापूर्ण == c) थांबा;
		c = ख_अक्षर_लो(source_file);
	}
}

पूर्णांक hex(पूर्णांक c, खाता* source_file)
{
	जर (in_set(c, "0123456789")) परत (c - 48);
	अन्यथा जर (in_set(c, "abcdef")) परत (c - 87);
	अन्यथा जर (in_set(c, "ABCDEF")) परत (c - 55);
	अन्यथा जर (in_set(c, "#;")) ओळ_Comment(source_file);
	परत -1;
}

पूर्णांक hold;
पूर्णांक toggle;
शून्य process_byte(अक्षर c, खाता* source_file, पूर्णांक लिख)
{
	जर(0 <= hex(c, source_file))
	{
		जर(toggle)
		{
			जर(लिख) ख_अक्षर_दो(((hold * 16)) + hex(c, source_file), output);
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

शून्य first_pass(खाता* आदान)
{
	toggle = FALSE;
	पूर्णांक c;
	साठी(c = ख_अक्षर_लो(आदान); खातापूर्ण != c; c = ख_अक्षर_लो(आदान))
	{
		/* Check for and deal with label */
		जर(':' == c)
		{
			c = storeLabel(आदान, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		जर(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(आदान);
			जर ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(आदान);
			}
		}
		अन्यथा process_byte(c, आदान, FALSE);
	}
}

शून्य second_pass(खाता* आदान)
{
	toggle = FALSE;
	hold = 0;

	पूर्णांक c;
	साठी(c = ख_अक्षर_लो(आदान); खातापूर्ण != c; c = ख_अक्षर_लो(आदान))
	{
		जर(':' == c) c = Throwaway_token(आदान); /* Deal with : */
		अन्यथा जर(in_set(c, "!@$~%&")) storePointer(c, आदान);  /* Deal with !, @, $, ~, % and & */
		अन्यथा process_byte(c, आदान, TRUE);
	}
}

/* Standard C main program */
पूर्णांक main(पूर्णांक argc, अक्षर **argv)
{
	jump_table = रिक्त;
	Base_Address = 0x8048000;
	output = मानक_निकास;
	scratch = सुस्मृति(max_string + 1, आकार(अक्षर));

	खाता* आदान = ख_खोलो(argv[1], "r");
	output = मानक_निकास;
	जर(रिक्त != argv[2])
	{
		output = ख_खोलो(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(आदान);
	शुरुआत(आदान);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(आदान);

	/* Set file as executable */
	परत निकास_सफल;
}
