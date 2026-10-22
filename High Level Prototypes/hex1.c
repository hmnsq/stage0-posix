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

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ "../M2libc/bootstrappable.h"

#define max_string 4096

ഘടന entry
{
	ഘടന entry* next;
	unsigned target;
	അക്ഷരം name;
};


/* Globals */
FILE* output;
ഘടന entry* jump_table;
പൂർണ്ണസംഖ്യ ip;

unsigned GetTarget(അക്ഷരം c)
{
	ഘടന entry* i;
	വേണ്ടി(i = jump_table; ശൂന്യ != i; i = i->next)
	{
		എങ്കിൽ(c == i->name)
		{
			തിരികെ i->target;
		}
	}
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ storeLabel(FILE* source_file, പൂർണ്ണസംഖ്യ ip)
{
	ഘടന entry* entry = calloc(1, വലുപ്പം(ഘടന entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	തിരികെ -1;
}


ശൂന്യം outputPointer(പൂർണ്ണസംഖ്യ displacement, പൂർണ്ണസംഖ്യ സംഖ്യ_of_bytes)
{
	unsigned മൂല്യം = displacement;

	ആകുമ്പോൾ(സംഖ്യ_of_bytes > 0)
	{
		unsigned byte = മൂല്യം % 256;
		മൂല്യം = മൂല്യം / 256;
		fputc(byte, output);
		സംഖ്യ_of_bytes = സംഖ്യ_of_bytes - 1;
	}
}

ശൂന്യം Update_Pointer(അക്ഷരം ch)
{
	/* Calculate pointer size*/
	എങ്കിൽ(in_set(ch, "%")) ip = ip + 4;
	അല്ലെങ്കിൽ exit(EXIT_FAILURE);
}

ശൂന്യം storePointer(അക്ഷരം ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	പൂർണ്ണസംഖ്യ target = GetTarget(ch);
	പൂർണ്ണസംഖ്യ displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	എങ്കിൽ('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	അല്ലെങ്കിൽ exit(EXIT_FAILURE);
}

ശൂന്യം വരി_Comment(FILE* source_file)
{
	പൂർണ്ണസംഖ്യ c = fgetc(source_file);
	ആകുമ്പോൾ(!in_set(c, "\n\r"))
	{
		എങ്കിൽ(EOF == c) നിർത്തുക;
		c = fgetc(source_file);
	}
}

പൂർണ്ണസംഖ്യ hex(പൂർണ്ണസംഖ്യ c, FILE* source_file)
{
	എങ്കിൽ (in_set(c, "0123456789")) തിരികെ (c - 48);
	അല്ലെങ്കിൽ എങ്കിൽ (in_set(c, "abcdef")) തിരികെ (c - 87);
	അല്ലെങ്കിൽ എങ്കിൽ (in_set(c, "ABCDEF")) തിരികെ (c - 55);
	അല്ലെങ്കിൽ എങ്കിൽ (in_set(c, "#;")) വരി_Comment(source_file);
	തിരികെ -1;
}

പൂർണ്ണസംഖ്യ hold;
പൂർണ്ണസംഖ്യ toggle;
ശൂന്യം process_byte(അക്ഷരം c, FILE* source_file, പൂർണ്ണസംഖ്യ ലിഖ)
{
	എങ്കിൽ(0 <= hex(c, source_file))
	{
		എങ്കിൽ(toggle)
		{
			എങ്കിൽ(ലിഖ) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		അല്ലെങ്കിൽ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

ശൂന്യം first_pass(FILE* നിവേശം)
{
	toggle = FALSE;
	പൂർണ്ണസംഖ്യ c;
	വേണ്ടി(c = fgetc(നിവേശം); EOF != c; c = fgetc(നിവേശം))
	{
		/* Check for and deal with label */
		എങ്കിൽ(':' == c)
		{
			c = storeLabel(നിവേശം, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		എങ്കിൽ(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(നിവേശം);
		}
		അല്ലെങ്കിൽ process_byte(c, നിവേശം, FALSE);
	}
}

ശൂന്യം second_pass(FILE* നിവേശം)
{
	toggle = FALSE;
	hold = 0;

	പൂർണ്ണസംഖ്യ c;
	വേണ്ടി(c = fgetc(നിവേശം); EOF != c; c = fgetc(നിവേശം))
	{
		എങ്കിൽ(':' == c) c = fgetc(നിവേശം); /* Deal with : */
		അല്ലെങ്കിൽ എങ്കിൽ(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		അല്ലെങ്കിൽ process_byte(c, നിവേശം, TRUE);
	}
}

/* Standard C main program */
പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
	jump_table = ശൂന്യ;
	output = stdout;

	FILE* നിവേശം = fopen(argv[1], "r");
	output = stdout;
	എങ്കിൽ(ശൂന്യ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(നിവേശം);
	rewind(നിവേശം);

	/* Fix all the references*/
	ip = 0;
	second_pass(നിവേശം);

	/* Set file as executable */
	തിരികെ EXIT_SUCCESS;
}
