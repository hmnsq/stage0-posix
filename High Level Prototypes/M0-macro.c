/* Copyright (C) 2016 Jeremiah Orians
 * This file is part of stage0.
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
#ضمن <stdint.h>
#ضمن <stdbool.h>
#ضمن <string.h>
#تعریف_کن max_string 63

FILE* منبع_file;
bool Reached_EOF;

بنية Token
{
	بنية Token* بعدی;
	uint8_t type;
	حرف* متن;
	حرف* عبارت;
};

تعداد type
{
	macro = 1,
	str = (1 << 1)
};

بنية Token* newToken()
{
	بنية Token* p;

	p = حافظه_صفر_بده (1, حجم (بنية Token));
	إذا (عدم == p)
	{
		fprintf (stderr, "calloc failed.\n");
		خروج (EXIT_FAILURE);
	}

	أرجع p;
}

بنية Token* جمع_کن_Token(بنية Token* سر, بنية Token* p)
{
	إذا(عدم == سر)
	{
		أرجع p;
	}
	إذا(عدم == سر->بعدی)
	{
		سر->بعدی = p;
	}
	وإلا
	{
		جمع_کن_Token(سر->بعدی, p);
	}
	أرجع سر;
}

فراغ purge_سطر_Comment()
{
	صحيح c = fgetc(منبع_file);
	طالما((10 != c) && (13 != c))
	{
		c = fgetc(منبع_file);
	}
}

حرف* store_atom(حرف c)
{
	حرف* store = حافظه_صفر_بده(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		ch = fgetc(منبع_file);
		i = i + 1;
	} طالما ((9 != ch) && (10 != ch) && (32 != ch));

	أرجع store;
}

حرف* store_string(حرف c)
{
	حرف* store = حافظه_صفر_بده(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		i = i + 1;
		ch = fgetc(منبع_file);
	} طالما(ch != c);

	أرجع store;
}

بنية Token* Tokenize_سطر(بنية Token* سر)
{

	int32_t c;
	c = fgetc(منبع_file);

	إذا((35 == c) || (59 == c))
	{
		purge_سطر_Comment();
		أرجع Tokenize_سطر(سر);
	}

	إذا((9 == c) || (10 == c) || (32 == c))
	{
		أرجع Tokenize_سطر(سر);
	}

	بنية Token* p = newToken();
	إذا(-1 == c)
	{
		Reached_EOF = true;
		حرر(p);
		أرجع سر;
	}
	وإلا إذا((34 == c) || (39 == c))
	{
		p->متن = store_string(c);
		p->type = str;
	}
	وإلا
	{
		p->متن = store_atom(c);
	}

	أرجع جمع_کن_Token(سر, p);
}

فراغ مجموعه_عبارت(بنية Token* p, حرف تطابق[], حرف Exp[])
{
	/* Leave macros alone */
	إذا((p->type & macro))
	{
		مجموعه_عبارت(p->بعدی, تطابق, Exp);
		أرجع;
	}

	/* Only if there is an exact match replace */
	إذا(0 == مقایسه_رشته(p->متن, تطابق, max_string))
	{
		p->عبارت = Exp;
	}

	إذا(عدم != p->بعدی)
	{
		مجموعه_عبارت(p->بعدی, تطابق, Exp);
	}

}

فراغ identify_macros(بنية Token* p)
{
	إذا(0 == مقایسه_رشته(p->متن, "DEFINE", max_string))
	{
		p->type = macro;
		p->متن = p->بعدی->متن;
		إذا(p->بعدی->بعدی->type & str)
		{
			p->عبارت = p->بعدی->بعدی->متن + 1;
		}
		وإلا
		{
			p->عبارت = p->بعدی->بعدی->متن;
		}
		p->بعدی = p->بعدی->بعدی->بعدی;
	}

	إذا(عدم != p->بعدی)
	{
		identify_macros(p->بعدی);
	}
}

فراغ سطر_macro(بنية Token* p)
{
	إذا(p->type & macro)
	{
		مجموعه_عبارت(p->بعدی, p->متن, p->عبارت);
	}

	إذا(عدم != p->بعدی)
	{
		سطر_macro(p->بعدی);
	}
}

فراغ hexify_string(بنية Token* p)
{
	حرف جدول[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	صحيح i = ((strnlen(p->متن + 1 , max_string)/4) + 1) * 8;

	حرف* d = حافظه_صفر_بده(max_string, حجم(حرف));
	p->عبارت = d;

	طالما(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	طالما( i < max_string)
	{
		إذا(0 == p->متن[i+1])
		{
			i = max_string;
		}
		وإلا
		{
			d[2*i]  = جدول[p->متن[i+1] / 16];
			d[2*i + 1] = جدول[p->متن[i+1] % 16];
			i = i + 1;
		}
	}
}

فراغ process_string(بنية Token* p)
{
	إذا(p->type & str)
	{
		إذا('\'' == p->متن[0])
		{
			p->عبارت = p->متن + 1;
		}
		وإلا إذا('"' == p->متن[0])
		{
			hexify_string(p);
		}
	}

	إذا(عدم != p->بعدی)
	{
		process_string(p->بعدی);
	}
}


فراغ preserve_other(بنية Token* p)
{
	إذا(عدم != p->بعدی)
	{
		preserve_other(p->بعدی);
	}

	إذا((عدم == p->عبارت) && !(p->type & macro))
	{
		p->عبارت = p->متن;
	}
}

uint16_t numerate_string(حرف a[])
{
	حرف *ptr;
	أرجع (uint16_t)strtol(a, &ptr, 0);
}

فراغ eval_immediates(بنية Token* p)
{
	إذا(عدم != p->بعدی)
	{
		eval_immediates(p->بعدی);
	}

	إذا((عدم == p->عبارت) && !(p->type & macro))
	{
		uint16_t مقدار;
		مقدار = numerate_string(p->متن);

		إذا(('0' == p->متن[0]) || (0 != مقدار))
		{
			حرف* c = حافظه_صفر_بده(5, حجم(حرف));
			sprintf(c, "%04x", مقدار);
			p->عبارت = c;
		}
	}
}

فراغ print_hex(بنية Token* p)
{
	إذا(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->عبارت);
	}

	إذا(عدم != p->بعدی)
	{
		print_hex(p->بعدی);
	}
	وإلا
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
صحيح اصلی(صحيح argc, حرف **argv)
{
	/* Make sure we have a program tape to run */
	إذا (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		أرجع EXIT_FAILURE;
	}

	منبع_file = fopen(argv[1], "r");

	Reached_EOF = false;
	بنية Token* سر = عدم;
	طالما(!Reached_EOF)
	{
		سر = Tokenize_سطر(سر);
	}

	identify_macros(سر);
	سطر_macro(سر);
	process_string(سر);
	eval_immediates(سر);
	preserve_other(سر);
	print_hex(سر);

	أرجع EXIT_SUCCESS;
}
