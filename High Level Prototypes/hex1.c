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

#تعریف_کن max_string 4096

بنية مدخل
{
	بنية مدخل* بعدی;
	بیعلامت هدف;
	حرف name;
};


/* Globals */
FILE* output;
بنية مدخل* jump_جدول;
صحيح ip;

بیعلامت Get_هدف(حرف c)
{
	بنية مدخل* i;
	لكل(i = jump_جدول; عدم != i; i = i->بعدی)
	{
		إذا(c == i->name)
		{
			أرجع i->هدف;
		}
	}
	أرجع 0;
}

صحيح storeLabel(FILE* منبع_file, صحيح ip)
{
	بنية مدخل* مدخل = حافظه_صفر_بده(1, حجم(بنية مدخل));

	/* Ensure we have target address */
	مدخل->هدف = ip;

	/* Prepend to list */
	مدخل->بعدی = jump_جدول;
	jump_جدول = مدخل;

	/* Store string */
	مدخل->name = fgetc(منبع_file);
	أرجع -1;
}


فراغ output_اشاره_گر(صحيح displacement, صحيح عدد_از_بایتها)
{
	بیعلامت مقدار = displacement;

	طالما(عدد_از_بایتها > 0)
	{
		بیعلامت byte = مقدار % 256;
		مقدار = مقدار / 256;
		fputc(byte, output);
		عدد_از_بایتها = عدد_از_بایتها - 1;
	}
}

فراغ Update_اشاره_گر(حرف ch)
{
	/* Calculate pointer size*/
	إذا(درون_مجموعه(ch, "%")) ip = ip + 4;
	وإلا خروج(EXIT_FAILURE);
}

فراغ store_اشاره_گر(حرف ch)
{
	/* Get string of pointer */
	Update_اشاره_گر(ch);

	/* Lookup token */
	صحيح هدف = Get_هدف(ch);
	صحيح displacement;

	/* Change relative base address to :<base> */
	displacement = (هدف - ip);

	/* output calculated difference */
	إذا('%' == ch) output_اشاره_گر(displacement, 4);  /* Deal with % */
	وإلا خروج(EXIT_FAILURE);
}

فراغ سطر_Comment(FILE* منبع_file)
{
	صحيح c = fgetc(منبع_file);
	طالما(!درون_مجموعه(c, "\n\r"))
	{
		إذا(EOF == c) اكسر;
		c = fgetc(منبع_file);
	}
}

صحيح hex(صحيح c, FILE* منبع_file)
{
	إذا (درون_مجموعه(c, "0123456789")) أرجع (c - 48);
	وإلا إذا (درون_مجموعه(c, "abcdef")) أرجع (c - 87);
	وإلا إذا (درون_مجموعه(c, "ABCDEF")) أرجع (c - 55);
	وإلا إذا (درون_مجموعه(c, "#;")) سطر_Comment(منبع_file);
	أرجع -1;
}

صحيح hold;
صحيح toggle;
فراغ process_byte(حرف c, FILE* منبع_file, صحيح اكتب)
{
	إذا(0 <= hex(c, منبع_file))
	{
		إذا(toggle)
		{
			إذا(اكتب) fputc(((hold * 16)) + hex(c, منبع_file), output);
			ip = ip + 1;
			hold = 0;
		}
		وإلا
		{
			hold = hex(c, منبع_file);
		}
		toggle = !toggle;
	}
}

فراغ اولی_pass(FILE* درون_داد)
{
	toggle = FALSE;
	صحيح c;
	لكل(c = fgetc(درون_داد); EOF != c; c = fgetc(درون_داد))
	{
		/* Check for and deal with label */
		إذا(':' == c)
		{
			c = storeLabel(درون_داد, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		إذا(درون_مجموعه(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_اشاره_گر(c);
			c = fgetc(درون_داد);
		}
		وإلا process_byte(c, درون_داد, FALSE);
	}
}

فراغ دومی_pass(FILE* درون_داد)
{
	toggle = FALSE;
	hold = 0;

	صحيح c;
	لكل(c = fgetc(درون_داد); EOF != c; c = fgetc(درون_داد))
	{
		إذا(':' == c) c = fgetc(درون_داد); /* Deal with : */
		وإلا إذا(درون_مجموعه(c, "%")) store_اشاره_گر(c);  /* Deal with %*/
		وإلا process_byte(c, درون_داد, TRUE);
	}
}

/* Standard C main program */
صحيح اصلی(صحيح argc, حرف **argv)
{
	jump_جدول = عدم;
	output = stdout;

	FILE* درون_داد = fopen(argv[1], "r");
	output = stdout;
	إذا(عدم != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	اولی_pass(درون_داد);
	rewind(درون_داد);

	/* Fix all the references*/
	ip = 0;
	دومی_pass(درون_داد);

	/* Set file as executable */
	أرجع EXIT_SUCCESS;
}
