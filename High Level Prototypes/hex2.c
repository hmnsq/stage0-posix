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
	unsigned target;
	वर्ण* name;
};


/* Globals */
FILE* output;
संरचना entry* jump_table;
पूर्णाङ्क Base_Address;
पूर्णाङ्क ip;
वर्ण* scratch;

पूर्णाङ्क consume_token(FILE* source_file)
{
	पूर्णाङ्क i = 0;
	पूर्णाङ्क c = fgetc(source_file);
	याव(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		सचे(EOF == c) भङ्ग;
	}

	पटिनिवत्त c;
}

पूर्णाङ्क Throwaway_token(FILE* source_file)
{
	पूर्णाङ्क c;
	करोहि
	{
		c = fgetc(source_file);
		सचे(EOF == c) भङ्ग;
	} याव(!in_set(c, " \t\n>"));

	पटिनिवत्त c;
}

पूर्णाङ्क length(वर्ण* s)
{
	पूर्णाङ्क i = 0;
	याव(0 != s[i]) i = i + 1;
	पटिनिवत्त i;
}

शून्य Clear_Scratch(वर्ण* s)
{
	करोहि
	{
		s[0] = 0;
		s = s + 1;
	} याव(0 != s[0]);
}

शून्य Copy_String(वर्ण* a, वर्ण* b)
{
	याव(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(वर्ण* c)
{
	संरचना entry* i;
	अत्थाय(i = jump_table; रिक्त != i; i = i->next)
	{
		सचे(match(c, i->name))
		{
			पटिनिवत्त i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
}

पूर्णाङ्क storeLabel(FILE* source_file, पूर्णाङ्क ip)
{
	संरचना entry* entry = calloc(1, परिमाण(संरचना entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	पूर्णाङ्क c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, परिमाण(वर्ण));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	पटिनिवत्त c;
}

शून्य outputPointer(पूर्णाङ्क displacement, पूर्णाङ्क सङ्ख्या_of_bytes)
{
	unsigned मानम् = displacement;

	याव(सङ्ख्या_of_bytes > 0)
	{
		unsigned byte = मानम् % 256;
		मानम् = मानम् / 256;
		fputc(byte, output);
		सङ्ख्या_of_bytes = सङ्ख्या_of_bytes - 1;
	}
}

शून्य Update_Pointer(वर्ण ch)
{
	/* Calculate pointer size*/
	सचे(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	अञ्ञथा सचे(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	अञ्ञथा सचे('!' == ch) ip = ip + 1; /* Deal with ! */
	अञ्ञथा exit(EXIT_FAILURE);
}

शून्य storePointer(वर्ण ch, FILE* source_file)
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
	सचे ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	अञ्ञथा
	{
		displacement = base - target;
	}

	/* output calculated difference */
	सचे('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	अञ्ञथा सचे('$' == ch) outputPointer(target, 2); /* Deal with $ */
	अञ्ञथा सचे('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	अञ्ञथा सचे('&' == ch) outputPointer(target, 4); /* Deal with & */
	अञ्ञथा सचे('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	अञ्ञथा exit(EXIT_FAILURE);
}

शून्य पङ्क्तिः_Comment(FILE* source_file)
{
	पूर्णाङ्क c = fgetc(source_file);
	याव(!in_set(c, "\n\r"))
	{
		सचे(EOF == c) भङ्ग;
		c = fgetc(source_file);
	}
}

पूर्णाङ्क hex(पूर्णाङ्क c, FILE* source_file)
{
	सचे (in_set(c, "0123456789")) पटिनिवत्त (c - 48);
	अञ्ञथा सचे (in_set(c, "abcdef")) पटिनिवत्त (c - 87);
	अञ्ञथा सचे (in_set(c, "ABCDEF")) पटिनिवत्त (c - 55);
	अञ्ञथा सचे (in_set(c, "#;")) पङ्क्तिः_Comment(source_file);
	पटिनिवत्त -1;
}

पूर्णाङ्क hold;
पूर्णाङ्क toggle;
शून्य process_byte(वर्ण c, FILE* source_file, पूर्णाङ्क लिख)
{
	सचे(0 <= hex(c, source_file))
	{
		सचे(toggle)
		{
			सचे(लिख) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		अञ्ञथा
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
	अत्थाय(c = fgetc(निवेशः); EOF != c; c = fgetc(निवेशः))
	{
		/* Check for and deal with label */
		सचे(':' == c)
		{
			c = storeLabel(निवेशः, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		सचे(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(निवेशः);
			सचे ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(निवेशः);
			}
		}
		अञ्ञथा process_byte(c, निवेशः, FALSE);
	}
}

शून्य second_pass(FILE* निवेशः)
{
	toggle = FALSE;
	hold = 0;

	पूर्णाङ्क c;
	अत्थाय(c = fgetc(निवेशः); EOF != c; c = fgetc(निवेशः))
	{
		सचे(':' == c) c = Throwaway_token(निवेशः); /* Deal with : */
		अञ्ञथा सचे(in_set(c, "!@$~%&")) storePointer(c, निवेशः);  /* Deal with !, @, $, ~, % and & */
		अञ्ञथा process_byte(c, निवेशः, TRUE);
	}
}

/* Standard C main program */
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
	jump_table = रिक्त;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, परिमाण(वर्ण));

	FILE* निवेशः = fopen(argv[1], "r");
	output = stdout;
	सचे(रिक्त != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(निवेशः);
	rewind(निवेशः);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(निवेशः);

	/* Set file as executable */
	पटिनिवत्त EXIT_SUCCESS;
}
