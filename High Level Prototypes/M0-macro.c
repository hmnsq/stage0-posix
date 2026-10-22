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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <stdint.h>
#شامل <stdbool.h>
#شامل <string.h>
#تعریف_کرو max_string 63

FILE* ماخذ_file;
bool Reached_EOF;

ساخت Token
{
	ساخت Token* اگلا;
	uint8_t type;
	حرف* متن;
	حرف* عبارت;
};

شمار type
{
	macro = 1,
	str = (1 << 1)
};

ساخت Token* newToken()
{
	ساخت Token* p;

	p = صفر_حافظہ_دو (1, حجم (ساخت Token));
	اگر (تہی == p)
	{
		fprintf (stderr, "calloc failed.\n");
		باہر_نکلو (EXIT_FAILURE);
	}

	واپس p;
}

ساخت Token* جوڑو_Token(ساخت Token* سر, ساخت Token* p)
{
	اگر(تہی == سر)
	{
		واپس p;
	}
	اگر(تہی == سر->اگلا)
	{
		سر->اگلا = p;
	}
	ورنہ
	{
		جوڑو_Token(سر->اگلا, p);
	}
	واپس سر;
}

خالی purge_سطر_Comment()
{
	صحیح c = fgetc(ماخذ_file);
	جب((10 != c) && (13 != c))
	{
		c = fgetc(ماخذ_file);
	}
}

حرف* store_atom(حرف c)
{
	حرف* store = صفر_حافظہ_دو(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	کرو
	{
		store[i] = (حرف)ch;
		ch = fgetc(ماخذ_file);
		i = i + 1;
	} جب ((9 != ch) && (10 != ch) && (32 != ch));

	واپس store;
}

حرف* store_string(حرف c)
{
	حرف* store = صفر_حافظہ_دو(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	کرو
	{
		store[i] = (حرف)ch;
		i = i + 1;
		ch = fgetc(ماخذ_file);
	} جب(ch != c);

	واپس store;
}

ساخت Token* Tokenize_سطر(ساخت Token* سر)
{

	int32_t c;
	c = fgetc(ماخذ_file);

	اگر((35 == c) || (59 == c))
	{
		purge_سطر_Comment();
		واپس Tokenize_سطر(سر);
	}

	اگر((9 == c) || (10 == c) || (32 == c))
	{
		واپس Tokenize_سطر(سر);
	}

	ساخت Token* p = newToken();
	اگر(-1 == c)
	{
		Reached_EOF = true;
		آزاد(p);
		واپس سر;
	}
	ورنہ اگر((34 == c) || (39 == c))
	{
		p->متن = store_string(c);
		p->type = str;
	}
	ورنہ
	{
		p->متن = store_atom(c);
	}

	واپس جوڑو_Token(سر, p);
}

خالی سیٹ_عبارت(ساخت Token* p, حرف مطابقت[], حرف Exp[])
{
	/* Leave macros alone */
	اگر((p->type & macro))
	{
		سیٹ_عبارت(p->اگلا, مطابقت, Exp);
		واپس;
	}

	/* Only if there is an exact match replace */
	اگر(0 == لڑی_موازنہ(p->متن, مطابقت, max_string))
	{
		p->عبارت = Exp;
	}

	اگر(تہی != p->اگلا)
	{
		سیٹ_عبارت(p->اگلا, مطابقت, Exp);
	}

}

خالی identify_macros(ساخت Token* p)
{
	اگر(0 == لڑی_موازنہ(p->متن, "DEFINE", max_string))
	{
		p->type = macro;
		p->متن = p->اگلا->متن;
		اگر(p->اگلا->اگلا->type & str)
		{
			p->عبارت = p->اگلا->اگلا->متن + 1;
		}
		ورنہ
		{
			p->عبارت = p->اگلا->اگلا->متن;
		}
		p->اگلا = p->اگلا->اگلا->اگلا;
	}

	اگر(تہی != p->اگلا)
	{
		identify_macros(p->اگلا);
	}
}

خالی سطر_macro(ساخت Token* p)
{
	اگر(p->type & macro)
	{
		سیٹ_عبارت(p->اگلا, p->متن, p->عبارت);
	}

	اگر(تہی != p->اگلا)
	{
		سطر_macro(p->اگلا);
	}
}

خالی hexify_string(ساخت Token* p)
{
	حرف جدول[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	صحیح i = ((strnlen(p->متن + 1 , max_string)/4) + 1) * 8;

	حرف* d = صفر_حافظہ_دو(max_string, حجم(حرف));
	p->عبارت = d;

	جب(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	جب( i < max_string)
	{
		اگر(0 == p->متن[i+1])
		{
			i = max_string;
		}
		ورنہ
		{
			d[2*i]  = جدول[p->متن[i+1] / 16];
			d[2*i + 1] = جدول[p->متن[i+1] % 16];
			i = i + 1;
		}
	}
}

خالی process_string(ساخت Token* p)
{
	اگر(p->type & str)
	{
		اگر('\'' == p->متن[0])
		{
			p->عبارت = p->متن + 1;
		}
		ورنہ اگر('"' == p->متن[0])
		{
			hexify_string(p);
		}
	}

	اگر(تہی != p->اگلا)
	{
		process_string(p->اگلا);
	}
}


خالی preserve_other(ساخت Token* p)
{
	اگر(تہی != p->اگلا)
	{
		preserve_other(p->اگلا);
	}

	اگر((تہی == p->عبارت) && !(p->type & macro))
	{
		p->عبارت = p->متن;
	}
}

uint16_t numerate_string(حرف a[])
{
	حرف *ptr;
	واپس (uint16_t)strtol(a, &ptr, 0);
}

خالی eval_immediates(ساخت Token* p)
{
	اگر(تہی != p->اگلا)
	{
		eval_immediates(p->اگلا);
	}

	اگر((تہی == p->عبارت) && !(p->type & macro))
	{
		uint16_t قدر;
		قدر = numerate_string(p->متن);

		اگر(('0' == p->متن[0]) || (0 != قدر))
		{
			حرف* c = صفر_حافظہ_دو(5, حجم(حرف));
			sprintf(c, "%04x", قدر);
			p->عبارت = c;
		}
	}
}

خالی print_hex(ساخت Token* p)
{
	اگر(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->عبارت);
	}

	اگر(تہی != p->اگلا)
	{
		print_hex(p->اگلا);
	}
	ورنہ
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
صحیح مین(صحیح argc, حرف **argv)
{
	/* Make sure we have a program tape to run */
	اگر (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		واپس EXIT_FAILURE;
	}

	ماخذ_file = fopen(argv[1], "r");

	Reached_EOF = false;
	ساخت Token* سر = تہی;
	جب(!Reached_EOF)
	{
		سر = Tokenize_سطر(سر);
	}

	identify_macros(سر);
	سطر_macro(سر);
	process_string(سر);
	eval_immediates(سر);
	preserve_other(سر);
	print_hex(سر);

	واپس EXIT_SUCCESS;
}
