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

#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <unistd.h>
#समावेश <sys/stat.h>
#समावेश "../M2libc/bootstrappable.h"

#घोषणा max_string 4096

काष्ठा entry
{
	काष्ठा entry* अग्र;
	अचिन्हित target;
	अक्षर* name;
};


/* Globals */
खाता* output;
काष्ठा entry* jump_table;
पूर्णांक Base_Address;
पूर्णांक ip;
अक्षर* scratch;

पूर्णांक consume_token(खाता* source_file)
{
	पूर्णांक i = 0;
	पूर्णांक c = ख_अक्षर_लो(source_file);
	जबतक(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = ख_अक्षर_लो(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		अगर(खातापूर्ण == c) अवरोध;
	}

	वापस c;
}

पूर्णांक Throwaway_token(खाता* source_file)
{
	पूर्णांक c;
	करो
	{
		c = ख_अक्षर_लो(source_file);
		अगर(खातापूर्ण == c) अवरोध;
	} जबतक(!in_set(c, " \t\n>"));

	वापस c;
}

पूर्णांक लंबाई(अक्षर* s)
{
	पूर्णांक i = 0;
	जबतक(0 != s[i]) i = i + 1;
	वापस i;
}

व्योम Clear_Scratch(अक्षर* s)
{
	करो
	{
		s[0] = 0;
		s = s + 1;
	} जबतक(0 != s[0]);
}

व्योम Copy_String(अक्षर* a, अक्षर* b)
{
	जबतक(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

अचिन्हित GetTarget(अक्षर* c)
{
	काष्ठा entry* i;
	क्रम(i = jump_table; शून्य != i; i = i->अग्र)
	{
		अगर(match(c, i->name))
		{
			वापस i->target;
		}
	}
	ख_माला_दो("Target label ", मानक_त्रुटि);
	ख_माला_दो(c, मानक_त्रुटि);
	ख_माला_दो(" is not valid\n", मानक_त्रुटि);
	निकास(निकास_त्रुटि);
}

पूर्णांक storeLabel(खाता* source_file, पूर्णांक ip)
{
	काष्ठा entry* entry = सुस्मृति(1, माप(काष्ठा entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->अग्र = jump_table;
	jump_table = entry;

	/* Store string */
	पूर्णांक c = consume_token(source_file);
	entry->name = सुस्मृति(लंबाई(scratch) + 1, माप(अक्षर));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	वापस c;
}

व्योम outputPointer(पूर्णांक displacement, पूर्णांक संख्या_of_bytes)
{
	अचिन्हित मूल्य = displacement;

	जबतक(संख्या_of_bytes > 0)
	{
		अचिन्हित byte = मूल्य % 256;
		मूल्य = मूल्य / 256;
		ख_अक्षर_दो(byte, output);
		संख्या_of_bytes = संख्या_of_bytes - 1;
	}
}

व्योम Update_Pointer(अक्षर ch)
{
	/* Calculate pointer size*/
	अगर(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	अन्यथा अगर(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	अन्यथा अगर('!' == ch) ip = ip + 1; /* Deal with ! */
	अन्यथा निकास(निकास_त्रुटि);
}

व्योम storePointer(अक्षर ch, खाता* source_file)
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
	अगर ('>' == base_sep_p)
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
	अगर('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	अन्यथा अगर('$' == ch) outputPointer(target, 2); /* Deal with $ */
	अन्यथा अगर('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	अन्यथा अगर('&' == ch) outputPointer(target, 4); /* Deal with & */
	अन्यथा अगर('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	अन्यथा निकास(निकास_त्रुटि);
}

व्योम पंक्ति_Comment(खाता* source_file)
{
	पूर्णांक c = ख_अक्षर_लो(source_file);
	जबतक(!in_set(c, "\n\r"))
	{
		अगर(खातापूर्ण == c) अवरोध;
		c = ख_अक्षर_लो(source_file);
	}
}

पूर्णांक hex(पूर्णांक c, खाता* source_file)
{
	अगर (in_set(c, "0123456789")) वापस (c - 48);
	अन्यथा अगर (in_set(c, "abcdef")) वापस (c - 87);
	अन्यथा अगर (in_set(c, "ABCDEF")) वापस (c - 55);
	अन्यथा अगर (in_set(c, "#;")) पंक्ति_Comment(source_file);
	वापस -1;
}

पूर्णांक hold;
पूर्णांक toggle;
व्योम process_byte(अक्षर c, खाता* source_file, पूर्णांक ग_लिखो)
{
	अगर(0 <= hex(c, source_file))
	{
		अगर(toggle)
		{
			अगर(ग_लिखो) ख_अक्षर_दो(((hold * 16)) + hex(c, source_file), output);
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

व्योम first_pass(खाता* निवेश)
{
	toggle = FALSE;
	पूर्णांक c;
	क्रम(c = ख_अक्षर_लो(निवेश); खातापूर्ण != c; c = ख_अक्षर_लो(निवेश))
	{
		/* Check for and deal with label */
		अगर(':' == c)
		{
			c = storeLabel(निवेश, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		अगर(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(निवेश);
			अगर ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(निवेश);
			}
		}
		अन्यथा process_byte(c, निवेश, FALSE);
	}
}

व्योम second_pass(खाता* निवेश)
{
	toggle = FALSE;
	hold = 0;

	पूर्णांक c;
	क्रम(c = ख_अक्षर_लो(निवेश); खातापूर्ण != c; c = ख_अक्षर_लो(निवेश))
	{
		अगर(':' == c) c = Throwaway_token(निवेश); /* Deal with : */
		अन्यथा अगर(in_set(c, "!@$~%&")) storePointer(c, निवेश);  /* Deal with !, @, $, ~, % and & */
		अन्यथा process_byte(c, निवेश, TRUE);
	}
}

/* Standard C main program */
पूर्णांक मुख्य(पूर्णांक argc, अक्षर **argv)
{
	jump_table = शून्य;
	Base_Address = 0x8048000;
	output = मानक_निकास;
	scratch = सुस्मृति(max_string + 1, माप(अक्षर));

	खाता* निवेश = ख_खोलो(argv[1], "r");
	output = मानक_निकास;
	अगर(शून्य != argv[2])
	{
		output = ख_खोलो(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(निवेश);
	शुरुआत(निवेश);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(निवेश);

	/* Set file as executable */
	वापस निकास_सफल;
}
