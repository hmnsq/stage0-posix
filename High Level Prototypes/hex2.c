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
	حرف* name;
};


/* Globals */
FILE* output;
بنية ثبت* jump_جدول;
صحيح بنسټ_پته;
صحيح ip;
حرف* scratch;

صحيح consume_token(FILE* سرچینه_file)
{
	صحيح i = 0;
	صحيح c = fgetc(سرچینه_file);
	طالما(!دننه_سېټ(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(سرچینه_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		إذا(EOF == c) اكسر;
	}

	أرجع c;
}

صحيح Throwaway_token(FILE* سرچینه_file)
{
	صحيح c;
	افعل
	{
		c = fgetc(سرچینه_file);
		إذا(EOF == c) اكسر;
	} طالما(!دننه_سېټ(c, " \t\n>"));

	أرجع c;
}

صحيح اوږدتیا(حرف* s)
{
	صحيح i = 0;
	طالما(0 != s[i]) i = i + 1;
	أرجع i;
}

فراغ Clear_Scratch(حرف* s)
{
	افعل
	{
		s[0] = 0;
		s = s + 1;
	} طالما(0 != s[0]);
}

فراغ Copy_String(حرف* a, حرف* b)
{
	طالما(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

بینښه Get_موخه(حرف* c)
{
	بنية ثبت* i;
	لكل(i = jump_جدول; عدم != i; i = i->بل)
	{
		إذا(سمون(c, i->name))
		{
			أرجع i->موخه;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	وتل(EXIT_FAILURE);
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
	صحيح c = consume_token(سرچینه_file);
	ثبت->name = حافظه_صفر_ورکړه(اوږدتیا(scratch) + 1, حجم(حرف));
	Copy_String(scratch, ثبت->name);
	Clear_Scratch(scratch);

	أرجع c;
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
	إذا(دننه_سېټ(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	وإلا إذا(دننه_سېټ(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	وإلا إذا('!' == ch) ip = ip + 1; /* Deal with ! */
	وإلا وتل(EXIT_FAILURE);
}

فراغ store_اشاره_ګر(حرف ch, FILE* سرچینه_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_اشاره_ګر(ch);
	صحيح بنسټ_sep_p = consume_token(سرچینه_file);

	/* Lookup token */
	صحيح موخه = Get_موخه(scratch);
	صحيح displacement;

	صحيح بنسټ = ip;

	/* Change relative base address to :<base> */
	إذا ('>' == بنسټ_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (سرچینه_file);
		بنسټ = Get_موخه (scratch);

		/* Force universality of behavior */
		displacement = (موخه - بنسټ);
	}
	وإلا
	{
		displacement = بنسټ - موخه;
	}

	/* output calculated difference */
	إذا('!' == ch) output_اشاره_ګر(displacement, 1); /* Deal with ! */
	وإلا إذا('$' == ch) output_اشاره_ګر(موخه, 2); /* Deal with $ */
	وإلا إذا('@' == ch) output_اشاره_ګر(displacement, 2); /* Deal with @ */
	وإلا إذا('&' == ch) output_اشاره_ګر(موخه, 4); /* Deal with & */
	وإلا إذا('%' == ch) output_اشاره_ګر(displacement, 4);  /* Deal with % */
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
		إذا(دننه_سېټ(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_اشاره_ګر(c);
			c = Throwaway_token(ننوت);
			إذا ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(ننوت);
			}
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
		إذا(':' == c) c = Throwaway_token(ننوت); /* Deal with : */
		وإلا إذا(دننه_سېټ(c, "!@$~%&")) store_اشاره_ګر(c, ننوت);  /* Deal with !, @, $, ~, % and & */
		وإلا process_byte(c, ننوت, TRUE);
	}
}

/* Standard C main program */
صحيح اصلي(صحيح argc, حرف **argv)
{
	jump_جدول = عدم;
	بنسټ_پته = 0x8048000;
	output = stdout;
	scratch = حافظه_صفر_ورکړه(max_string + 1, حجم(حرف));

	FILE* ننوت = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = بنسټ_پته;
	لومړی_pass(ننوت);
	rewind(ننوت);

	/* Fix all the references*/
	ip = بنسټ_پته;
	دوهم_pass(ننوت);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
