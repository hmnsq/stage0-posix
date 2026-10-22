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
#تعریف_کړه max_string 63

FILE* سرچینه_file;
bool Reached_EOF;

بنية Token
{
	بنية Token* بل;
	uint8_t type;
	حرف* متن;
	حرف* افاده;
};

تعداد type
{
	macro = 1,
	str = (1 << 1)
};

بنية Token* newToken()
{
	بنية Token* p;

	p = حافظه_صفر_ورکړه (1, حجم (بنية Token));
	إذا (عدم == p)
	{
		fprintf (stderr, "calloc failed.\n");
		وتل (EXIT_FAILURE);
	}

	أرجع p;
}

بنية Token* جمع_کړه_Token(بنية Token* سر, بنية Token* p)
{
	إذا(عدم == سر)
	{
		أرجع p;
	}
	إذا(عدم == سر->بل)
	{
		سر->بل = p;
	}
	وإلا
	{
		جمع_کړه_Token(سر->بل, p);
	}
	أرجع سر;
}

فراغ purge_کرښه_Comment()
{
	صحيح c = fgetc(سرچینه_file);
	طالما((10 != c) && (13 != c))
	{
		c = fgetc(سرچینه_file);
	}
}

حرف* store_atom(حرف c)
{
	حرف* store = حافظه_صفر_ورکړه(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		ch = fgetc(سرچینه_file);
		i = i + 1;
	} طالما ((9 != ch) && (10 != ch) && (32 != ch));

	أرجع store;
}

حرف* store_string(حرف c)
{
	حرف* store = حافظه_صفر_ورکړه(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		i = i + 1;
		ch = fgetc(سرچینه_file);
	} طالما(ch != c);

	أرجع store;
}

بنية Token* Tokenize_کرښه(بنية Token* سر)
{

	int32_t c;
	c = fgetc(سرچینه_file);

	إذا((35 == c) || (59 == c))
	{
		purge_کرښه_Comment();
		أرجع Tokenize_کرښه(سر);
	}

	إذا((9 == c) || (10 == c) || (32 == c))
	{
		أرجع Tokenize_کرښه(سر);
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

	أرجع جمع_کړه_Token(سر, p);
}

فراغ سېټ_افاده(بنية Token* p, حرف سمون[], حرف Exp[])
{
	/* Leave macros alone */
	إذا((p->type & macro))
	{
		سېټ_افاده(p->بل, سمون, Exp);
		أرجع;
	}

	/* Only if there is an exact match replace */
	إذا(0 == مزي_پرتله(p->متن, سمون, max_string))
	{
		p->افاده = Exp;
	}

	إذا(عدم != p->بل)
	{
		سېټ_افاده(p->بل, سمون, Exp);
	}

}

فراغ identify_macros(بنية Token* p)
{
	إذا(0 == مزي_پرتله(p->متن, "DEFINE", max_string))
	{
		p->type = macro;
		p->متن = p->بل->متن;
		إذا(p->بل->بل->type & str)
		{
			p->افاده = p->بل->بل->متن + 1;
		}
		وإلا
		{
			p->افاده = p->بل->بل->متن;
		}
		p->بل = p->بل->بل->بل;
	}

	إذا(عدم != p->بل)
	{
		identify_macros(p->بل);
	}
}

فراغ کرښه_macro(بنية Token* p)
{
	إذا(p->type & macro)
	{
		سېټ_افاده(p->بل, p->متن, p->افاده);
	}

	إذا(عدم != p->بل)
	{
		کرښه_macro(p->بل);
	}
}

فراغ hexify_string(بنية Token* p)
{
	حرف جدول[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	صحيح i = ((strnlen(p->متن + 1 , max_string)/4) + 1) * 8;

	حرف* d = حافظه_صفر_ورکړه(max_string, حجم(حرف));
	p->افاده = d;

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
			p->افاده = p->متن + 1;
		}
		وإلا إذا('"' == p->متن[0])
		{
			hexify_string(p);
		}
	}

	إذا(عدم != p->بل)
	{
		process_string(p->بل);
	}
}


فراغ preserve_other(بنية Token* p)
{
	إذا(عدم != p->بل)
	{
		preserve_other(p->بل);
	}

	إذا((عدم == p->افاده) && !(p->type & macro))
	{
		p->افاده = p->متن;
	}
}

uint16_t numerate_string(حرف a[])
{
	حرف *ptr;
	أرجع (uint16_t)strtol(a, &ptr, 0);
}

فراغ eval_immediates(بنية Token* p)
{
	إذا(عدم != p->بل)
	{
		eval_immediates(p->بل);
	}

	إذا((عدم == p->افاده) && !(p->type & macro))
	{
		uint16_t ارزښت;
		ارزښت = numerate_string(p->متن);

		إذا(('0' == p->متن[0]) || (0 != ارزښت))
		{
			حرف* c = حافظه_صفر_ورکړه(5, حجم(حرف));
			sprintf(c, "%04x", ارزښت);
			p->افاده = c;
		}
	}
}

فراغ print_hex(بنية Token* p)
{
	إذا(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->افاده);
	}

	إذا(عدم != p->بل)
	{
		print_hex(p->بل);
	}
	وإلا
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
صحيح اصلي(صحيح argc, حرف **argv)
{
	/* Make sure we have a program tape to run */
	إذا (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		أرجع EXIT_FAILURE;
	}

	سرچینه_file = fopen(argv[1], "r");

	Reached_EOF = false;
	بنية Token* سر = عدم;
	طالما(!Reached_EOF)
	{
		سر = Tokenize_کرښه(سر);
	}

	identify_macros(سر);
	کرښه_macro(سر);
	process_string(سر);
	eval_immediates(سر);
	preserve_other(سر);
	print_hex(سر);

	أرجع EXIT_SUCCESS;
}
