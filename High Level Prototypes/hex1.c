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
	वर्ण name;
};


/* Globals */
FILE* output;
संरचना entry* jump_table;
पूर्णाङ्क ip;

unsigned GetTarget(वर्ण c)
{
	संरचना entry* i;
	अत्थाय(i = jump_table; रिक्त != i; i = i->next)
	{
		सचे(c == i->name)
		{
			पटिनिवत्त i->target;
		}
	}
	पटिनिवत्त 0;
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
	entry->name = fgetc(source_file);
	पटिनिवत्त -1;
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
	सचे(in_set(ch, "%")) ip = ip + 4;
	अञ्ञथा exit(EXIT_FAILURE);
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
	सचे('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
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
		सचे(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(निवेशः);
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
		सचे(':' == c) c = fgetc(निवेशः); /* Deal with : */
		अञ्ञथा सचे(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		अञ्ञथा process_byte(c, निवेशः, TRUE);
	}
}

/* Standard C main program */
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
	jump_table = रिक्त;
	output = stdout;

	FILE* निवेशः = fopen(argv[1], "r");
	output = stdout;
	सचे(रिक्त != argv[2])
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
	पटिनिवत्त EXIT_SUCCESS;
}
