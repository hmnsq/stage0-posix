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

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <unistd.h>
#ضمن <sys/stat.h>
#ضمن "../M2libc/bootstrappable.h"

#define max_string 4096

بنية entry
{
	بنية entry* next;
	unsigned target;
	حرف name;
};


/* Globals */
FILE* output;
بنية entry* jump_table;
صحيح ip;

unsigned GetTarget(حرف c)
{
	بنية entry* i;
	لكل(i = jump_table; عدم != i; i = i->next)
	{
		إذا(c == i->name)
		{
			أرجع i->target;
		}
	}
	أرجع 0;
}

صحيح storeLabel(FILE* source_file, صحيح ip)
{
	بنية entry* entry = calloc(1, حجم(بنية entry));

	/* Ensure we have target address */
	entry->target = ip;

	/* Prepend to list */
	entry->next = jump_table;
	jump_table = entry;

	/* Store string */
	entry->name = fgetc(source_file);
	أرجع -1;
}


فراغ outputPointer(صحيح displacement, صحيح لمب_of_bytes)
{
	unsigned درج = displacement;

	طالما(لمب_of_bytes > 0)
	{
		unsigned byte = درج % 256;
		درج = درج / 256;
		fputc(byte, output);
		لمب_of_bytes = لمب_of_bytes - 1;
	}
}

فراغ Update_Pointer(حرف ch)
{
	/* Calculate pointer size*/
	إذا(in_set(ch, "%")) ip = ip + 4;
	وإلا exit(EXIT_FAILURE);
}

فراغ storePointer(حرف ch)
{
	/* Get string of pointer */
	Update_Pointer(ch);

	/* Lookup token */
	صحيح target = GetTarget(ch);
	صحيح displacement;

	/* Change relative base address to :<base> */
	displacement = (target - ip);

	/* output calculated difference */
	إذا('%' == ch) outputPointer(displacement, 4);  /* Deal with % */
	وإلا exit(EXIT_FAILURE);
}

فراغ لي_Comment(FILE* source_file)
{
	صحيح c = fgetc(source_file);
	طالما(!in_set(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(source_file);
	}
}

صحيح hex(صحيح c, FILE* source_file)
{
	إذا (in_set(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (in_set(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (in_set(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (in_set(c, "#;")) لي_Comment(source_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* source_file, صحيح اكتب)
{
	إذا(0 <= hex(c, source_file))
	{
		إذا(toggle)
		{
			إذا(اكتب) fputc(((hold * 16)) + hex(c, source_file), output);
			ip = ip + 1;
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, source_file);
		}
		toggle = !toggle;
	}
}

فراغ first_pass(FILE* شغرو)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(شغرو); EOF != c; c = fgetc(شغرو))
	{
		/* Check for and deal with label */
		إذا(':' == c)
		{
			c = storeLabel(شغرو, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		إذا(in_set(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_Pointer(c);
			c = fgetc(شغرو);
		}
		وإلا process_byte(c, شغرو, FALSE);
	}
}

فراغ second_pass(FILE* شغرو)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(شغرو); EOF != c; c = fgetc(شغرو))
	{
		إذا(':' == c) c = fgetc(شغرو); /* Deal with : */
		وإلا إذا(in_set(c, "%")) storePointer(c);  /* Deal with %*/
		وإلا process_byte(c, شغرو, TRUE);
	}
}

/* Standard C main program */
صحيح main(صحيح argc, حرف **argv)
{
	jump_table = عدم;
	output = stdout;

	FILE* شغرو = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	first_pass(شغرو);
	rewind(شغرو);

	/* Fix all the references*/
	ip = 0;
	second_pass(شغرو);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
