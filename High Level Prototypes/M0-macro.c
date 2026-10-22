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
#وصف_ڪريو max_string 63

FILE* ذريعو_file;
bool Reached_EOF;

بنية Token
{
	بنية Token* ايندڙ;
	uint8_t type;
	حرف* متن;
	حرف* اظهار;
};

تعداد type
{
	macro = 1,
	str = (1 << 1)
};

بنية Token* newToken()
{
	بنية Token* p;

	p = صفر_يادگيري_ڏيو (1, حجم (بنية Token));
	إذا (عدم == p)
	{
		fprintf (stderr, "calloc failed.\n");
		ٻاهر_نڪرو (EXIT_FAILURE);
	}

	أرجع p;
}

بنية Token* جوڙيو_Token(بنية Token* سر, بنية Token* p)
{
	إذا(عدم == سر)
	{
		أرجع p;
	}
	إذا(عدم == سر->ايندڙ)
	{
		سر->ايندڙ = p;
	}
	وإلا
	{
		جوڙيو_Token(سر->ايندڙ, p);
	}
	أرجع سر;
}

فراغ purge_سٽ_Comment()
{
	صحيح c = fgetc(ذريعو_file);
	طالما((10 != c) && (13 != c))
	{
		c = fgetc(ذريعو_file);
	}
}

حرف* store_atom(حرف c)
{
	حرف* store = صفر_يادگيري_ڏيو(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		ch = fgetc(ذريعو_file);
		i = i + 1;
	} طالما ((9 != ch) && (10 != ch) && (32 != ch));

	أرجع store;
}

حرف* store_string(حرف c)
{
	حرف* store = صفر_يادگيري_ڏيو(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		i = i + 1;
		ch = fgetc(ذريعو_file);
	} طالما(ch != c);

	أرجع store;
}

بنية Token* Tokenize_سٽ(بنية Token* سر)
{

	int32_t c;
	c = fgetc(ذريعو_file);

	إذا((35 == c) || (59 == c))
	{
		purge_سٽ_Comment();
		أرجع Tokenize_سٽ(سر);
	}

	إذا((9 == c) || (10 == c) || (32 == c))
	{
		أرجع Tokenize_سٽ(سر);
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

	أرجع جوڙيو_Token(سر, p);
}

فراغ سيٽ_اظهار(بنية Token* p, حرف ميلاپ[], حرف Exp[])
{
	/* Leave macros alone */
	إذا((p->type & macro))
	{
		سيٽ_اظهار(p->ايندڙ, ميلاپ, Exp);
		أرجع;
	}

	/* Only if there is an exact match replace */
	إذا(0 == لڙي_ڀيٽ(p->متن, ميلاپ, max_string))
	{
		p->اظهار = Exp;
	}

	إذا(عدم != p->ايندڙ)
	{
		سيٽ_اظهار(p->ايندڙ, ميلاپ, Exp);
	}

}

فراغ identify_macros(بنية Token* p)
{
	إذا(0 == لڙي_ڀيٽ(p->متن, "DEFINE", max_string))
	{
		p->type = macro;
		p->متن = p->ايندڙ->متن;
		إذا(p->ايندڙ->ايندڙ->type & str)
		{
			p->اظهار = p->ايندڙ->ايندڙ->متن + 1;
		}
		وإلا
		{
			p->اظهار = p->ايندڙ->ايندڙ->متن;
		}
		p->ايندڙ = p->ايندڙ->ايندڙ->ايندڙ;
	}

	إذا(عدم != p->ايندڙ)
	{
		identify_macros(p->ايندڙ);
	}
}

فراغ سٽ_macro(بنية Token* p)
{
	إذا(p->type & macro)
	{
		سيٽ_اظهار(p->ايندڙ, p->متن, p->اظهار);
	}

	إذا(عدم != p->ايندڙ)
	{
		سٽ_macro(p->ايندڙ);
	}
}

فراغ hexify_string(بنية Token* p)
{
	حرف جدول[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	صحيح i = ((strnlen(p->متن + 1 , max_string)/4) + 1) * 8;

	حرف* d = صفر_يادگيري_ڏيو(max_string, حجم(حرف));
	p->اظهار = d;

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
			p->اظهار = p->متن + 1;
		}
		وإلا إذا('"' == p->متن[0])
		{
			hexify_string(p);
		}
	}

	إذا(عدم != p->ايندڙ)
	{
		process_string(p->ايندڙ);
	}
}


فراغ preserve_other(بنية Token* p)
{
	إذا(عدم != p->ايندڙ)
	{
		preserve_other(p->ايندڙ);
	}

	إذا((عدم == p->اظهار) && !(p->type & macro))
	{
		p->اظهار = p->متن;
	}
}

uint16_t numerate_string(حرف a[])
{
	حرف *ptr;
	أرجع (uint16_t)strtol(a, &ptr, 0);
}

فراغ eval_immediates(بنية Token* p)
{
	إذا(عدم != p->ايندڙ)
	{
		eval_immediates(p->ايندڙ);
	}

	إذا((عدم == p->اظهار) && !(p->type & macro))
	{
		uint16_t قدر;
		قدر = numerate_string(p->متن);

		إذا(('0' == p->متن[0]) || (0 != قدر))
		{
			حرف* c = صفر_يادگيري_ڏيو(5, حجم(حرف));
			sprintf(c, "%04x", قدر);
			p->اظهار = c;
		}
	}
}

فراغ print_hex(بنية Token* p)
{
	إذا(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->اظهار);
	}

	إذا(عدم != p->ايندڙ)
	{
		print_hex(p->ايندڙ);
	}
	وإلا
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
صحيح مکيه(صحيح argc, حرف **argv)
{
	/* Make sure we have a program tape to run */
	إذا (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		أرجع EXIT_FAILURE;
	}

	ذريعو_file = fopen(argv[1], "r");

	Reached_EOF = false;
	بنية Token* سر = عدم;
	طالما(!Reached_EOF)
	{
		سر = Tokenize_سٽ(سر);
	}

	identify_macros(سر);
	سٽ_macro(سر);
	process_string(سر);
	eval_immediates(سر);
	preserve_other(سر);
	print_hex(سر);

	أرجع EXIT_SUCCESS;
}
