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

/* Globals */
FILE* output;

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
ശൂന്യം process_byte(അക്ഷരം c, FILE* source_file)
{
	എങ്കിൽ(0 <= hex(c, source_file))
	{
		എങ്കിൽ(toggle)
		{
			fputc(((hold * 16)) + hex(c, source_file), output);
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
		process_byte(c, നിവേശം);
	}
}

/* Standard C main program */
പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
	output = stdout;

	FILE* നിവേശം = fopen(argv[1], "r");
	output = stdout;
	എങ്കിൽ(ശൂന്യ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* do the work */
	first_pass(നിവേശം);

	/* Set file as executable */
	തിരികെ EXIT_SUCCESS;
}
