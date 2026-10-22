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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../M2libc/bootstrappable.h"

#define max_string 4096

sa_nracanaa entry
{
	sa_nracanaa entry* next;
	unsigned target;
	var_na name;
};


/* Globals */
FILE* output;
sa_nracanaa entry* jump_table;
ຈຳນວນເຕັມ ip;

unsigned GetTarget(var_na c)
{
	sa_nracanaa entry* i;
	ສຳລັບ(i = jump_table; NULL != i; i = i->next)
	{
		ຖ້າ(c == i->name)
		{
			ສົ່ງຄືນ i->target;
		}
	}
	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ storeLabel(FILE* source_file, ຈຳນວນເຕັມ ip)
{
	sa_nracanaa entry* entry = calloc(1, parimaa_na(sa_nracanaa entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	ສົ່ງຄືນ -1;
}


void outputPointer(ຈຳນວນເຕັມ displacement, ຈຳນວນເຕັມ number_of_bytes)
{
	unsigned value = displacement;

	ໃນຂະນະທີ່(number_of_bytes > 0)
	{
		unsigned byte = value % 256;
		value = value / 256;
		fputc(byte, output);
		number_of_bytes = number_of_bytes - 1;
	}
}

void Update_Pointer(var_na ch)
{
	/* Calculate pointer size*/
	ຖ້າ(in_set(ch, "%")) ip = ip + 4;
	ບໍ່ດັ່ງນັ້ນ exit(EXIT_FAILURE);
}

void storePointer(var_na ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	ຈຳນວນເຕັມ target = GetTarget(ch);
	ຈຳນວນເຕັມ displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	ຖ້າ('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	ບໍ່ດັ່ງນັ້ນ exit(EXIT_FAILURE);
}

void line_Comment(FILE* source_file)
{
	ຈຳນວນເຕັມ c = fgetc(source_file);
	ໃນຂະນະທີ່(!in_set(c, "\n\r"))
	{
		ຖ້າ(EOF == c) bha_nga;
		c = fgetc(source_file);
	}
}

ຈຳນວນເຕັມ hex(ຈຳນວນເຕັມ c, FILE* source_file)
{
	ຖ້າ (in_set(c, "0123456789")) ສົ່ງຄືນ (c - 48);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_set(c, "abcdef")) ສົ່ງຄືນ (c - 87);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_set(c, "ABCDEF")) ສົ່ງຄືນ (c - 55);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (in_set(c, "#;")) line_Comment(source_file);
	ສົ່ງຄືນ -1;
}

ຈຳນວນເຕັມ hold;
ຈຳນວນເຕັມ toggle;
void process_byte(var_na c, FILE* source_file, ຈຳນວນເຕັມ write)
{
	ຖ້າ(0 <= hex(c, source_file))
	{
		ຖ້າ(toggle)
		{
			ຖ້າ(write) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

void first_pass(FILE* input)
{
	toggle = FALSE;
	ຈຳນວນເຕັມ c;
	ສຳລັບ(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		/* Check for and deal with label */
		ຖ້າ(':' == c)
		{
			c = storeLabel(input, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ຖ້າ(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(input);
		}
		ບໍ່ດັ່ງນັ້ນ process_byte(c, input, FALSE);
	}
}

void second_pass(FILE* input)
{
	toggle = FALSE;
	hold = 0;

	ຈຳນວນເຕັມ c;
	ສຳລັບ(c = fgetc(input); EOF != c; c = fgetc(input))
	{
		ຖ້າ(':' == c) c = fgetc(input); /* Deal with : */
		ບໍ່ດັ່ງນັ້ນ ຖ້າ(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		ບໍ່ດັ່ງນັ້ນ process_byte(c, input, TRUE);
	}
}

/* Standard C main program */
ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc, var_na **argv)
{
	jump_table = NULL;
	output = stdout;

	FILE* input = fopen(argv[1], "r");
	output = stdout;
	ຖ້າ(NULL != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(input);
	rewind(input);

	/* Fix all the references*/
	ip = 0;
	second_pass(input);

	/* Set file as executable */
	ສົ່ງຄືນ EXIT_SUCCESS;
}
