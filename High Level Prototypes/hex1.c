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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <unistd.h>
#شامل <sys/stat.h>
#شامل "../M2libc/bootstrappable.h"

#تعریف_کرو max_string 4096

ساخت اندراج_2
{
	ساخت اندراج_2* اگلا;
	غیرعلامتی ہدف;
	حرف name;
};


/* Globals */
FILE* output;
ساخت اندراج_2* jump_جدول;
صحیح ip;

غیرعلامتی Get_ہدف(حرف c)
{
	ساخت اندراج_2* i;
	برائے(i = jump_جدول; تہی != i; i = i->اگلا)
	{
		اگر(c == i->name)
		{
			واپس i->ہدف;
		}
	}
	واپس 0;
}

صحیح storeLabel(FILE* ماخذ_file, صحیح ip)
{
	ساخت اندراج_2* اندراج_2 = صفر_حافظہ_دو(1, حجم(ساخت اندراج_2));

	/* Ensure we have target address */
	اندراج_2->ہدف = ip;

	/* Prepend to list */
	اندراج_2->اگلا = jump_جدول;
	jump_جدول = اندراج_2;

	/* Store string */
	اندراج_2->name = fgetc(ماخذ_file);
	واپس -1;
}


خالی output_اشارہ_گر(صحیح displacement, صحیح نمبر_کا_بائٹیں)
{
	غیرعلامتی قدر = displacement;

	جب(نمبر_کا_بائٹیں > 0)
	{
		غیرعلامتی byte = قدر % 256;
		قدر = قدر / 256;
		fputc(byte, output);
		نمبر_کا_بائٹیں = نمبر_کا_بائٹیں - 1;
	}
}

خالی Update_اشارہ_گر(حرف ch)
{
	/* Calculate pointer size*/
	اگر(اندر_سیٹ(ch, "%")) ip = ip + 4;
	ورنہ باہر_نکلو(EXIT_FAILURE);
}

خالی store_اشارہ_گر(حرف ch)
{
	/* Get string of pointer */
	Update_اشارہ_گر(ch);

	/* Lookup token */
	صحیح ہدف = Get_ہدف(ch);
	صحیح displacement;

	/* Change relative base address to :<base> */
	displacement = (ہدف - ip);

	/* output calculated difference */
	اگر('%' == ch) output_اشارہ_گر(displacement, 4);  /* Deal with % */
	ورنہ باہر_نکلو(EXIT_FAILURE);
}

خالی سطر_Comment(FILE* ماخذ_file)
{
	صحیح c = fgetc(ماخذ_file);
	جب(!اندر_سیٹ(c, "\n\r"))
	{
		اگر(EOF == c) توڑو;
		c = fgetc(ماخذ_file);
	}
}

صحیح hex(صحیح c, FILE* ماخذ_file)
{
	اگر (اندر_سیٹ(c, "0123456789")) واپس (c - 48);
	ورنہ اگر (اندر_سیٹ(c, "abcdef")) واپس (c - 87);
	ورنہ اگر (اندر_سیٹ(c, "ABCDEF")) واپس (c - 55);
	ورنہ اگر (اندر_سیٹ(c, "#;")) سطر_Comment(ماخذ_file);
	واپس -1;
}

صحیح hold;
صحیح toggle;
خالی process_byte(حرف c, FILE* ماخذ_file, صحیح اكتب)
{
	اگر(0 <= hex(c, ماخذ_file))
	{
		اگر(toggle)
		{
			اگر(اكتب) fputc(((hold * 16)) + hex(c, ماخذ_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ورنہ
		{
			hold = hex(c, ماخذ_file);
		}
		toggle = !toggle;
	}
}

خالی پہلا_pass(FILE* داخلہ)
{
	toggle = FALSE;
	صحیح c;
	برائے(c = fgetc(داخلہ); EOF != c; c = fgetc(داخلہ))
	{
		/* Check for and deal with label */
		اگر(':' == c)
		{
			c = storeLabel(داخلہ, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		اگر(اندر_سیٹ(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_اشارہ_گر(c);
			c = fgetc(داخلہ);
		}
		ورنہ process_byte(c, داخلہ, FALSE);
	}
}

خالی دوسرا_pass(FILE* داخلہ)
{
	toggle = FALSE;
	hold = 0;

	صحیح c;
	برائے(c = fgetc(داخلہ); EOF != c; c = fgetc(داخلہ))
	{
		اگر(':' == c) c = fgetc(داخلہ); /* Deal with : */
		ورنہ اگر(اندر_سیٹ(c, "%")) store_اشارہ_گر(c);  /* Deal with %*/
		ورنہ process_byte(c, داخلہ, TRUE);
	}
}

/* Standard C main program */
صحیح مین(صحیح argc, حرف **argv)
{
	jump_جدول = تہی;
	output = stdout;

	FILE* داخلہ = fopen(argv[1], "r");
	output = stdout;
	اگر(تہی != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	پہلا_pass(داخلہ);
	rewind(داخلہ);

	/* Fix all the references*/
	ip = 0;
	دوسرا_pass(داخلہ);

	/* Set file as executable */
	واپس EXIT_SUCCESS;
}
