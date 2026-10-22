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

সংরচনা Token
{
	সংরচনা Token* next;
	uint8_t type;
	বর্ণ* Text;
	বর্ণ* Expression;
};

গণনা type
{
	macro = 1,
	str = (1 << 1)
};

সংরচনা Token* newToken()
{
	সংরচনা Token* p;

	p = calloc (1, পরিমাণ (সংরচনা Token));
	যদি (রিক্ত == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	প্রত্যাগম p;
}

সংরচনা Token* addToken(সংরচনা Token* head, সংরচনা Token* p)
{
	যদি(রিক্ত == head)
	{
		প্রত্যাগম p;
	}
	যদি(রিক্ত == head->next)
	{
		head->next = p;
	}
	অন্যথা
	{
		addToken(head->next, p);
	}
	প্রত্যাগম head;
}

শূন্য purge_সারি_Comment()
{
	পূর্ণাঙ্ক c = fgetc(source_file);
	যাবত্((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

বর্ণ* store_atom(বর্ণ c)
{
	বর্ণ* store = calloc(max_string + 1, পরিমাণ(বর্ণ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	কুরু
	{
		store[i] = (বর্ণ)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} যাবত্ ((9 != ch) && (10 != ch) && (32 != ch));

	প্রত্যাগম store;
}

বর্ণ* store_string(বর্ণ c)
{
	বর্ণ* store = calloc(max_string + 1, পরিমাণ(বর্ণ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	কুরু
	{
		store[i] = (বর্ণ)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} যাবত্(ch != c);

	প্রত্যাগম store;
}

সংরচনা Token* Tokenize_সারি(সংরচনা Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	যদি((35 == c) || (59 == c))
	{
		purge_সারি_Comment();
		প্রত্যাগম Tokenize_সারি(head);
	}

	যদি((9 == c) || (10 == c) || (32 == c))
	{
		প্রত্যাগম Tokenize_সারি(head);
	}

	সংরচনা Token* p = newToken();
	যদি(-1 == c)
	{
		Reached_EOF = true;
		মুক্ত(p);
		প্রত্যাগম head;
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

	প্রত্যাগম addToken(head, p);
}

শূন্য setExpression(সংরচনা Token* p, বর্ণ match[], বর্ণ Exp[])
{
	/* Leave macros alone */
	যদি((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		প্রত্যাগম;
	}

	/* Only if there is an exact match replace */
	যদি(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	যদি(রিক্ত != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

শূন্য identify_macros(সংরচনা Token* p)
{
	যদি(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		যদি(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		অন্যথা
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	যদি(রিক্ত != p->next)
	{
		identify_macros(p->next);
	}
}

শূন্য সারি_macro(সংরচনা Token* p)
{
	যদি(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	যদি(রিক্ত != p->next)
	{
		সারি_macro(p->next);
	}
}

শূন্য hexify_string(সংরচনা Token* p)
{
	বর্ণ table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	পূর্ণাঙ্ক i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	বর্ণ* d = calloc(max_string, পরিমাণ(বর্ণ));
	p->Expression = d;

	যাবত্(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	যাবত্( i < max_string)
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

শূন্য process_string(সংরচনা Token* p)
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

	যদি(রিক্ত != p->next)
	{
		process_string(p->next);
	}
}


শূন্য preserve_other(সংরচনা Token* p)
{
	যদি(রিক্ত != p->next)
	{
		preserve_other(p->next);
	}

	যদি((রিক্ত == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(বর্ণ a[])
{
	বর্ণ *ptr;
	প্রত্যাগম (uint16_t)strtol(a, &ptr, 0);
}

শূন্য eval_immediates(সংরচনা Token* p)
{
	যদি(রিক্ত != p->next)
	{
		eval_immediates(p->next);
	}

	যদি((রিক্ত == p->Expression) && !(p->type & macro))
	{
		uint16_t মান;
		মান = numerate_string(p->Text);

		যদি(('0' == p->Text[0]) || (0 != মান))
		{
			বর্ণ* c = calloc(5, পরিমাণ(বর্ণ));
			sprintf(c, "%04x", মান);
			p->Expression = c;
		}
	}
}

শূন্য print_hex(সংরচনা Token* p)
{
	যদি(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	যদি(রিক্ত != p->next)
	{
		print_hex(p->next);
	}
	অন্যথা
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, বর্ণ **argv)
{
	/* Make sure we have a program tape to run */
	যদি (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		প্রত্যাগম EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	সংরচনা Token* head = রিক্ত;
	যাবত্(!Reached_EOF)
	{
		head = Tokenize_সারি(head);
	}

	identify_macros(head);
	সারি_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	প্রত্যাগম EXIT_SUCCESS;
}
