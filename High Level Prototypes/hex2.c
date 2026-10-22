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

#تعریف_کرو max_string 4096

بنية اندراج
{
	بنية اندراج* اگلا;
	بنانشانی نشانہ;
	حرف* name;
};


/* Globals */
FILE* output;
بنية اندراج* jump_جدول;
صحيح ادھار_پتہ;
صحيح ip;
حرف* scratch;

صحيح consume_token(FILE* سروت_file)
{
	صحيح i = 0;
	صحيح c = fgetc(سروت_file);
	طالما(!وچ_سیٹ(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(سروت_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		إذا(EOF == c) اكسر;
	}

	أرجع c;
}

صحيح Throwaway_token(FILE* سروت_file)
{
	صحيح c;
	افعل
	{
		c = fgetc(سروت_file);
		إذا(EOF == c) اكسر;
	} طالما(!وچ_سیٹ(c, " \t\n>"));

	أرجع c;
}

صحيح لمائی(حرف* s)
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

بنانشانی Get_نشانہ(حرف* c)
{
	بنية اندراج* i;
	لكل(i = jump_جدول; عدم != i; i = i->اگلا)
	{
		إذا(میل(c, i->name))
		{
			أرجع i->نشانہ;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	باہر_نکلو(EXIT_FAILURE);
}

صحيح storeLabel(FILE* سروت_file, صحيح ip)
{
	بنية اندراج* اندراج = صفر_حافظہ_دیو(1, حجم(بنية اندراج));

	/* Ensure we have target address */
	اندراج->نشانہ = ip;

	/* Prepend to list */
	اندراج->اگلا = jump_جدول;
	jump_جدول = اندراج;

	/* Store string */
	صحيح c = consume_token(سروت_file);
	اندراج->name = صفر_حافظہ_دیو(لمائی(scratch) + 1, حجم(حرف));
	Copy_String(scratch, اندراج->name);
	Clear_Scratch(scratch);

	أرجع c;
}

فراغ output_اشارہ_گر(صحيح displacement, صحيح نمبر_دا_بائٹاں)
{
	بنانشانی قدر = displacement;

	طالما(نمبر_دا_بائٹاں > 0)
	{
		بنانشانی byte = قدر % 256;
		قدر = قدر / 256;
		fputc(byte, output);
		نمبر_دا_بائٹاں = نمبر_دا_بائٹاں - 1;
	}
}

فراغ Update_اشارہ_گر(حرف ch)
{
	/* Calculate pointer size*/
	إذا(وچ_سیٹ(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	وإلا إذا(وچ_سیٹ(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	وإلا إذا('!' == ch) ip = ip + 1; /* Deal with ! */
	وإلا باہر_نکلو(EXIT_FAILURE);
}

فراغ store_اشارہ_گر(حرف ch, FILE* سروت_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_اشارہ_گر(ch);
	صحيح ادھار_sep_p = consume_token(سروت_file);

	/* Lookup token */
	صحيح نشانہ = Get_نشانہ(scratch);
	صحيح displacement;

	صحيح ادھار = ip;

	/* Change relative base address to :<base> */
	إذا ('>' == ادھار_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (سروت_file);
		ادھار = Get_نشانہ (scratch);

		/* Force universality of behavior */
		displacement = (نشانہ - ادھار);
	}
	وإلا
	{
		displacement = ادھار - نشانہ;
	}

	/* output calculated difference */
	إذا('!' == ch) output_اشارہ_گر(displacement, 1); /* Deal with ! */
	وإلا إذا('$' == ch) output_اشارہ_گر(نشانہ, 2); /* Deal with $ */
	وإلا إذا('@' == ch) output_اشارہ_گر(displacement, 2); /* Deal with @ */
	وإلا إذا('&' == ch) output_اشارہ_گر(نشانہ, 4); /* Deal with & */
	وإلا إذا('%' == ch) output_اشارہ_گر(displacement, 4);  /* Deal with % */
	وإلا باہر_نکلو(EXIT_FAILURE);
}

فراغ سطر_Comment(FILE* سروت_file)
{
	صحيح c = fgetc(سروت_file);
	طالما(!وچ_سیٹ(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(سروت_file);
	}
}

صحيح hex(صحيح c, FILE* سروت_file)
{
	إذا (وچ_سیٹ(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (وچ_سیٹ(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (وچ_سیٹ(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (وچ_سیٹ(c, "#;")) سطر_Comment(سروت_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* سروت_file, صحيح اكتب)
{
	إذا(0 <= hex(c, سروت_file))
	{
		إذا(toggle)
		{
			إذا(اكتب) fputc(((hold * 16)) + hex(c, سروت_file), output);
			ip = ip + 1;
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, سروت_file);
		}
		toggle = !toggle;
	}
}

فراغ پہلا_pass(FILE* داخلہ)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(داخلہ); EOF != c; c = fgetc(داخلہ))
	{
		/* Check for and deal with label */
		إذا(':' == c)
		{
			c = storeLabel(داخلہ, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		إذا(وچ_سیٹ(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_اشارہ_گر(c);
			c = Throwaway_token(داخلہ);
			إذا ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(داخلہ);
			}
		}
		وإلا process_byte(c, داخلہ, FALSE);
	}
}

فراغ دوجا_pass(FILE* داخلہ)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(داخلہ); EOF != c; c = fgetc(داخلہ))
	{
		إذا(':' == c) c = Throwaway_token(داخلہ); /* Deal with : */
		وإلا إذا(وچ_سیٹ(c, "!@$~%&")) store_اشارہ_گر(c, داخلہ);  /* Deal with !, @, $, ~, % and & */
		وإلا process_byte(c, داخلہ, TRUE);
	}
}

/* Standard C main program */
صحيح مکھ(صحيح argc, حرف **argv)
{
	jump_جدول = عدم;
	ادھار_پتہ = 0x8048000;
	output = stdout;
	scratch = صفر_حافظہ_دیو(max_string + 1, حجم(حرف));

	FILE* داخلہ = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = ادھار_پتہ;
	پہلا_pass(داخلہ);
	rewind(داخلہ);

	/* Fix all the references*/
	ip = ادھار_پتہ;
	دوجا_pass(داخلہ);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
