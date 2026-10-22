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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <stdbool.h>
#অন্তর্ভাব <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

কাঠামো Token
{
	কাঠামো Token* পরবর্তী;
	uint8_t type;
	অক্ষর* Text;
	অক্ষর* Expression;
};

গণনা type
{
	macro = 1,
	str = (1 << 1)
};

কাঠামো Token* newToken()
{
	কাঠামো Token* p;

	p = calloc (1, আকার (কাঠামো Token));
	যদি (ফাঁকা == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	ফেরত p;
}

কাঠামো Token* addToken(কাঠামো Token* শীর্ষ, কাঠামো Token* p)
{
	যদি(ফাঁকা == শীর্ষ)
	{
		ফেরত p;
	}
	যদি(ফাঁকা == শীর্ষ->পরবর্তী)
	{
		শীর্ষ->পরবর্তী = p;
	}
	অন্যথা
	{
		addToken(শীর্ষ->পরবর্তী, p);
	}
	ফেরত শীর্ষ;
}

শূন্য purge_সারি_Comment()
{
	পূর্ণসংখ্যা c = fgetc(source_file);
	যখন((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

অক্ষর* store_atom(অক্ষর c)
{
	অক্ষর* store = calloc(max_string + 1, আকার(অক্ষর));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	করো
	{
		store[i] = (অক্ষর)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} যখন ((9 != ch) && (10 != ch) && (32 != ch));

	ফেরত store;
}

অক্ষর* store_string(অক্ষর c)
{
	অক্ষর* store = calloc(max_string + 1, আকার(অক্ষর));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	করো
	{
		store[i] = (অক্ষর)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} যখন(ch != c);

	ফেরত store;
}

কাঠামো Token* Tokenize_সারি(কাঠামো Token* শীর্ষ)
{

	int32_t c;
	c = fgetc(source_file);

	যদি((35 == c) || (59 == c))
	{
		purge_সারি_Comment();
		ফেরত Tokenize_সারি(শীর্ষ);
	}

	যদি((9 == c) || (10 == c) || (32 == c))
	{
		ফেরত Tokenize_সারি(শীর্ষ);
	}

	কাঠামো Token* p = newToken();
	যদি(-1 == c)
	{
		Reached_EOF = true;
		মুক্ত(p);
		ফেরত শীর্ষ;
	}
	অন্যথা যদি((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	অন্যথা
	{
		p->Text = store_atom(c);
	}

	ফেরত addToken(শীর্ষ, p);
}

শূন্য setExpression(কাঠামো Token* p, অক্ষর match[], অক্ষর Exp[])
{
	/* Leave macros alone */
	যদি((p->type & macro))
	{
		setExpression(p->পরবর্তী, match, Exp);
		ফেরত;
	}

	/* Only if there is an exact match replace */
	যদি(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	যদি(ফাঁকা != p->পরবর্তী)
	{
		setExpression(p->পরবর্তী, match, Exp);
	}

}

শূন্য identify_macros(কাঠামো Token* p)
{
	যদি(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->পরবর্তী->Text;
		যদি(p->পরবর্তী->পরবর্তী->type & str)
		{
			p->Expression = p->পরবর্তী->পরবর্তী->Text + 1;
		}
		অন্যথা
		{
			p->Expression = p->পরবর্তী->পরবর্তী->Text;
		}
		p->পরবর্তী = p->পরবর্তী->পরবর্তী->পরবর্তী;
	}

	যদি(ফাঁকা != p->পরবর্তী)
	{
		identify_macros(p->পরবর্তী);
	}
}

শূন্য সারি_macro(কাঠামো Token* p)
{
	যদি(p->type & macro)
	{
		setExpression(p->পরবর্তী, p->Text, p->Expression);
	}

	যদি(ফাঁকা != p->পরবর্তী)
	{
		সারি_macro(p->পরবর্তী);
	}
}

শূন্য hexify_string(কাঠামো Token* p)
{
	অক্ষর table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	পূর্ণসংখ্যা i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	অক্ষর* d = calloc(max_string, আকার(অক্ষর));
	p->Expression = d;

	যখন(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	যখন( i < max_string)
	{
		যদি(0 == p->Text[i+1])
		{
			i = max_string;
		}
		অন্যথা
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

শূন্য process_string(কাঠামো Token* p)
{
	যদি(p->type & str)
	{
		যদি('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		অন্যথা যদি('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	যদি(ফাঁকা != p->পরবর্তী)
	{
		process_string(p->পরবর্তী);
	}
}


শূন্য preserve_other(কাঠামো Token* p)
{
	যদি(ফাঁকা != p->পরবর্তী)
	{
		preserve_other(p->পরবর্তী);
	}

	যদি((ফাঁকা == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(অক্ষর a[])
{
	অক্ষর *ptr;
	ফেরত (uint16_t)strtol(a, &ptr, 0);
}

শূন্য eval_immediates(কাঠামো Token* p)
{
	যদি(ফাঁকা != p->পরবর্তী)
	{
		eval_immediates(p->পরবর্তী);
	}

	যদি((ফাঁকা == p->Expression) && !(p->type & macro))
	{
		uint16_t মান;
		মান = numerate_string(p->Text);

		যদি(('0' == p->Text[0]) || (0 != মান))
		{
			অক্ষর* c = calloc(5, আকার(অক্ষর));
			sprintf(c, "%04x", মান);
			p->Expression = c;
		}
	}
}

শূন্য print_hex(কাঠামো Token* p)
{
	যদি(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	যদি(ফাঁকা != p->পরবর্তী)
	{
		print_hex(p->পরবর্তী);
	}
	অন্যথা
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
	/* Make sure we have a program tape to run */
	যদি (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		ফেরত EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	কাঠামো Token* শীর্ষ = ফাঁকা;
	যখন(!Reached_EOF)
	{
		শীর্ষ = Tokenize_সারি(শীর্ষ);
	}

	identify_macros(শীর্ষ);
	সারি_macro(শীর্ষ);
	process_string(শীর্ষ);
	eval_immediates(শীর্ষ);
	preserve_other(শীর্ষ);
	print_hex(শীর্ষ);

	ফেরত EXIT_SUCCESS;
}
