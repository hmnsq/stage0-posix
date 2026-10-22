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

#وصف_ڪريو max_string 4096

بنية داخلا_2
{
	بنية داخلا_2* ايندڙ;
	بنانشان هدف;
	حرف* name;
};


/* Globals */
FILE* output;
بنية داخلا_2* jump_جدول;
صحيح بنياد_پتو;
صحيح ip;
حرف* scratch;

صحيح consume_token(FILE* ذريعو_file)
{
	صحيح i = 0;
	صحيح c = fgetc(ذريعو_file);
	طالما(!اندريون_سيٽ(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(ذريعو_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		إذا(EOF == c) اكسر;
	}

	أرجع c;
}

صحيح Throwaway_token(FILE* ذريعو_file)
{
	صحيح c;
	افعل
	{
		c = fgetc(ذريعو_file);
		إذا(EOF == c) اكسر;
	} طالما(!اندريون_سيٽ(c, " \t\n>"));

	أرجع c;
}

صحيح ڊيگهائي(حرف* s)
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

بنانشان Get_هدف(حرف* c)
{
	بنية داخلا_2* i;
	لكل(i = jump_جدول; عدم != i; i = i->ايندڙ)
	{
		إذا(ميلاپ(c, i->name))
		{
			أرجع i->هدف;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	ٻاهر_نڪرو(EXIT_FAILURE);
}

صحيح storeLabel(FILE* ذريعو_file, صحيح ip)
{
	بنية داخلا_2* داخلا_2 = صفر_يادگيري_ڏيو(1, حجم(بنية داخلا_2));

	/* Ensure we have target address */
	داخلا_2->هدف = ip;

	/* Prepend to list */
	داخلا_2->ايندڙ = jump_جدول;
	jump_جدول = داخلا_2;

	/* Store string */
	صحيح c = consume_token(ذريعو_file);
	داخلا_2->name = صفر_يادگيري_ڏيو(ڊيگهائي(scratch) + 1, حجم(حرف));
	Copy_String(scratch, داخلا_2->name);
	Clear_Scratch(scratch);

	أرجع c;
}

فراغ output_اشارو(صحيح displacement, صحيح عدد_جو_بائيٽون)
{
	بنانشان قدر = displacement;

	طالما(عدد_جو_بائيٽون > 0)
	{
		بنانشان byte = قدر % 256;
		قدر = قدر / 256;
		fputc(byte, output);
		عدد_جو_بائيٽون = عدد_جو_بائيٽون - 1;
	}
}

فراغ Update_اشارو(حرف ch)
{
	/* Calculate pointer size*/
	إذا(اندريون_سيٽ(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	وإلا إذا(اندريون_سيٽ(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	وإلا إذا('!' == ch) ip = ip + 1; /* Deal with ! */
	وإلا ٻاهر_نڪرو(EXIT_FAILURE);
}

فراغ store_اشارو(حرف ch, FILE* ذريعو_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_اشارو(ch);
	صحيح بنياد_sep_p = consume_token(ذريعو_file);

	/* Lookup token */
	صحيح هدف = Get_هدف(scratch);
	صحيح displacement;

	صحيح بنياد = ip;

	/* Change relative base address to :<base> */
	إذا ('>' == بنياد_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (ذريعو_file);
		بنياد = Get_هدف (scratch);

		/* Force universality of behavior */
		displacement = (هدف - بنياد);
	}
	وإلا
	{
		displacement = بنياد - هدف;
	}

	/* output calculated difference */
	إذا('!' == ch) output_اشارو(displacement, 1); /* Deal with ! */
	وإلا إذا('$' == ch) output_اشارو(هدف, 2); /* Deal with $ */
	وإلا إذا('@' == ch) output_اشارو(displacement, 2); /* Deal with @ */
	وإلا إذا('&' == ch) output_اشارو(هدف, 4); /* Deal with & */
	وإلا إذا('%' == ch) output_اشارو(displacement, 4);  /* Deal with % */
	وإلا ٻاهر_نڪرو(EXIT_FAILURE);
}

فراغ سٽ_Comment(FILE* ذريعو_file)
{
	صحيح c = fgetc(ذريعو_file);
	طالما(!اندريون_سيٽ(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(ذريعو_file);
	}
}

صحيح hex(صحيح c, FILE* ذريعو_file)
{
	إذا (اندريون_سيٽ(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (اندريون_سيٽ(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (اندريون_سيٽ(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (اندريون_سيٽ(c, "#;")) سٽ_Comment(ذريعو_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* ذريعو_file, صحيح اكتب)
{
	إذا(0 <= hex(c, ذريعو_file))
	{
		إذا(toggle)
		{
			إذا(اكتب) fputc(((hold * 16)) + hex(c, ذريعو_file), output);
			ip = ip + 1;
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, ذريعو_file);
		}
		toggle = !toggle;
	}
}

فراغ پهريون_pass(FILE* داخلا)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(داخلا); EOF != c; c = fgetc(داخلا))
	{
		/* Check for and deal with label */
		إذا(':' == c)
		{
			c = storeLabel(داخلا, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		إذا(اندريون_سيٽ(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_اشارو(c);
			c = Throwaway_token(داخلا);
			إذا ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(داخلا);
			}
		}
		وإلا process_byte(c, داخلا, FALSE);
	}
}

فراغ ٻيو_pass(FILE* داخلا)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(داخلا); EOF != c; c = fgetc(داخلا))
	{
		إذا(':' == c) c = Throwaway_token(داخلا); /* Deal with : */
		وإلا إذا(اندريون_سيٽ(c, "!@$~%&")) store_اشارو(c, داخلا);  /* Deal with !, @, $, ~, % and & */
		وإلا process_byte(c, داخلا, TRUE);
	}
}

/* Standard C main program */
صحيح مکيه(صحيح argc, حرف **argv)
{
	jump_جدول = عدم;
	بنياد_پتو = 0x8048000;
	output = stdout;
	scratch = صفر_يادگيري_ڏيو(max_string + 1, حجم(حرف));

	FILE* داخلا = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = بنياد_پتو;
	پهريون_pass(داخلا);
	rewind(داخلا);

	/* Fix all the references*/
	ip = بنياد_پتو;
	ٻيو_pass(داخلا);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
