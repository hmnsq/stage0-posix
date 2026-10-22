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
	അക്ഷരം* name;
};


/* Globals */
FILE* output;
ഘടന entry* jump_table;
പൂർണ്ണസംഖ്യ Base_Address;
പൂർണ്ണസംഖ്യ ip;
അക്ഷരം* scratch;

പൂർണ്ണസംഖ്യ consume_token(FILE* source_file)
{
	പൂർണ്ണസംഖ്യ i = 0;
	പൂർണ്ണസംഖ്യ c = fgetc(source_file);
	ആകുമ്പോൾ(!in_set(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(source_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		എങ്കിൽ(EOF == c) നിർത്തുക;
	}

	തിരികെ c;
}

പൂർണ്ണസംഖ്യ Throwaway_token(FILE* source_file)
{
	പൂർണ്ണസംഖ്യ c;
	ചെയ്യുക
	{
		c = fgetc(source_file);
		എങ്കിൽ(EOF == c) നിർത്തുക;
	} ആകുമ്പോൾ(!in_set(c, " \t\n>"));

	തിരികെ c;
}

പൂർണ്ണസംഖ്യ length(അക്ഷരം* s)
{
	പൂർണ്ണസംഖ്യ i = 0;
	ആകുമ്പോൾ(0 != s[i]) i = i + 1;
	തിരികെ i;
}

ശൂന്യം Clear_Scratch(അക്ഷരം* s)
{
	ചെയ്യുക
	{
		s[0] = 0;
		s = s + 1;
	} ആകുമ്പോൾ(0 != s[0]);
}

ശൂന്യം Copy_String(അക്ഷരം* a, അക്ഷരം* b)
{
	ആകുമ്പോൾ(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

unsigned GetTarget(അക്ഷരം* c)
{
	ഘടന entry* i;
	വേണ്ടി(i = jump_table; ശൂന്യ != i; i = i->next)
	{
		എങ്കിൽ(match(c, i->name))
		{
			തിരികെ i->target;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	exit(EXIT_FAILURE);
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
	പൂർണ്ണസംഖ്യ c = consume_token(source_file);
	entry->name = calloc(length(scratch) + 1, വലുപ്പം(അക്ഷരം));
	Copy_String(scratch, entry->name);
	Clear_Scratch(scratch);

	തിരികെ c;
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
	എങ്കിൽ(in_set(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	അല്ലെങ്കിൽ എങ്കിൽ(in_set(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	അല്ലെങ്കിൽ എങ്കിൽ('!' == ch) ip = ip + 1; /* Deal with ! */
	അല്ലെങ്കിൽ exit(EXIT_FAILURE);
}

ശൂന്യം storePointer(അക്ഷരം ch, FILE* source_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_Pointer(ch);
	പൂർണ്ണസംഖ്യ base_sep_p = consume_token(source_file);

	/* Lookup token */
	പൂർണ്ണസംഖ്യ target = GetTarget(scratch);
	പൂർണ്ണസംഖ്യ displacement;

	പൂർണ്ണസംഖ്യ base = ip;

	/* Change relative base address to :<base> */
	എങ്കിൽ ('>' == base_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (source_file);
		base = GetTarget (scratch);

		/* Force universality of behavior */
		displacement = (target - base);
	}
	അല്ലെങ്കിൽ
	{
		displacement = base - target;
	}

	/* output calculated difference */
	എങ്കിൽ('!' == ch) outputPointer(displacement, 1); /* Deal with ! */
	അല്ലെങ്കിൽ എങ്കിൽ('$' == ch) outputPointer(target, 2); /* Deal with $ */
	അല്ലെങ്കിൽ എങ്കിൽ('@' == ch) outputPointer(displacement, 2); /* Deal with @ */
	അല്ലെങ്കിൽ എങ്കിൽ('&' == ch) outputPointer(target, 4); /* Deal with & */
	അല്ലെങ്കിൽ എങ്കിൽ('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
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
		എങ്കിൽ(in_set(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_Pointer(c);
			c = Throwaway_token(നിവേശം);
			എങ്കിൽ ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(നിവേശം);
			}
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
		എങ്കിൽ(':' == c) c = Throwaway_token(നിവേശം); /* Deal with : */
		അല്ലെങ്കിൽ എങ്കിൽ(in_set(c, "!@$~%&")) storePointer(c, നിവേശം);  /* Deal with !, @, $, ~, % and & */
		അല്ലെങ്കിൽ process_byte(c, നിവേശം, TRUE);
	}
}

/* Standard C main program */
പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
	jump_table = ശൂന്യ;
	Base_Address = 0x8048000;
	output = stdout;
	scratch = calloc(max_string + 1, വലുപ്പം(അക്ഷരം));

	FILE* നിവേശം = fopen(argv[1], "r");
	output = stdout;
	എങ്കിൽ(ശൂന്യ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = Base_Address;
	first_pass(നിവേശം);
	rewind(നിവേശം);

	/* Fix all the references*/
	ip = Base_Address;
	second_pass(നിവേശം);

	/* Set file as executable */
	തിരികെ EXIT_SUCCESS;
}
