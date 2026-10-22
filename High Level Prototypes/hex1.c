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

#تعریف_کړه max_string 4096

بنية ثبت
{
	بنية ثبت* بل;
	بینښه موخه;
	حرف name;
};


/* Globals */
FILE* output;
بنية ثبت* jump_جدول;
صحيح ip;

بینښه Get_موخه(حرف c)
{
	بنية ثبت* i;
	لكل(i = jump_جدول; عدم != i; i = i->بل)
	{
		إذا(c == i->name)
		{
			أرجع i->موخه;
		}
	}
	أرجع 0;
}

صحيح storeLabel(FILE* سرچینه_file, صحيح ip)
{
	بنية ثبت* ثبت = حافظه_صفر_ورکړه(1, حجم(بنية ثبت));

	/* Ensure we have target address */
	ثبت->موخه = ip;

	/* Prepend to list */
	ثبت->بل = jump_جدول;
	jump_جدول = ثبت;

	/* Store string */
	ثبت->name = fgetc(سرچینه_file);
	أرجع -1;
}


فراغ output_اشاره_ګر(صحيح displacement, صحيح شمېره_د_بایټونه)
{
	بینښه ارزښت = displacement;

	طالما(شمېره_د_بایټونه > 0)
	{
		بینښه byte = ارزښت % 256;
		ارزښت = ارزښت / 256;
		fputc(byte, output);
		شمېره_د_بایټونه = شمېره_د_بایټونه - 1;
	}
}

فراغ Update_اشاره_ګر(حرف ch)
{
	/* Calculate pointer size*/
	إذا(دننه_سېټ(ch, "%")) ip = ip + 4;
	وإلا وتل(EXIT_FAILURE);
}

فراغ store_اشاره_ګر(حرف ch)
{
	/* Get string of pointer */
	Update_اشاره_ګر(ch);

	/* Lookup token */
	صحيح موخه = Get_موخه(ch);
	صحيح displacement;

	/* Change relative base address to :<base> */
	displacement = (موخه - ip);

	/* output calculated difference */
	إذا('%' == ch) output_اشاره_ګر(displacement, 4);  /* Deal with % */
	وإلا وتل(EXIT_FAILURE);
}

فراغ کرښه_Comment(FILE* سرچینه_file)
{
	صحيح c = fgetc(سرچینه_file);
	طالما(!دننه_سېټ(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(سرچینه_file);
	}
}

صحيح hex(صحيح c, FILE* سرچینه_file)
{
	إذا (دننه_سېټ(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (دننه_سېټ(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (دننه_سېټ(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (دننه_سېټ(c, "#;")) کرښه_Comment(سرچینه_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* سرچینه_file, صحيح اكتب)
{
	إذا(0 <= hex(c, سرچینه_file))
	{
		إذا(toggle)
		{
			إذا(اكتب) fputc(((hold * 16)) + hex(c, سرچینه_file), output);
			ip = ip + 1;
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, سرچینه_file);
		}
		toggle = !toggle;
	}
}

فراغ لومړی_pass(FILE* ننوت)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(ننوت); EOF != c; c = fgetc(ننوت))
	{
		/* Check for and deal with label */
		إذا(':' == c)
		{
			c = storeLabel(ننوت, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		إذا(دننه_سېټ(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_اشاره_ګر(c);
			c = fgetc(ننوت);
		}
		وإلا process_byte(c, ننوت, FALSE);
	}
}

فراغ دوهم_pass(FILE* ننوت)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(ننوت); EOF != c; c = fgetc(ننوت))
	{
		إذا(':' == c) c = fgetc(ننوت); /* Deal with : */
		وإلا إذا(دننه_سېټ(c, "%")) store_اشاره_ګر(c);  /* Deal with %*/
		وإلا process_byte(c, ننوت, TRUE);
	}
}

/* Standard C main program */
صحيح اصلي(صحيح argc, حرف **argv)
{
	jump_جدول = عدم;
	output = stdout;

	FILE* ننوت = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	لومړی_pass(ننوت);
	rewind(ننوت);

	/* Fix all the references*/
	ip = 0;
	دوهم_pass(ننوت);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
