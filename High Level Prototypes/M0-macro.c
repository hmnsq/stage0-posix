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
#define max_string 63

FILE* source_file;
bool Reached_EOF;

ساختار Token
{
	ساختار Token* next;
	uint8_t type;
	نویسه* Text;
	نویسه* Expression;
};

شمارش type
{
	macro = 1,
	str = (1 << 1)
};

ساختار Token* newToken()
{
	ساختار Token* p;

	p = calloc (1, اندازه (ساختار Token));
	اگر (پوچ == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	بازگشت p;
}

ساختار Token* addToken(ساختار Token* head, ساختار Token* p)
{
	اگر(پوچ == head)
	{
		بازگشت p;
	}
	اگر(پوچ == head->next)
	{
		head->next = p;
	}
	وگرنه
	{
		addToken(head->next, p);
	}
	بازگشت head;
}

تهی purge_سطر_Comment()
{
	صحیح c = fgetc(source_file);
	وقتی((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

نویسه* store_atom(نویسه c)
{
	نویسه* store = calloc(max_string + 1, اندازه(نویسه));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	انجامبده
	{
		store[i] = (نویسه)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} وقتی ((9 != ch) && (10 != ch) && (32 != ch));

	بازگشت store;
}

نویسه* store_string(نویسه c)
{
	نویسه* store = calloc(max_string + 1, اندازه(نویسه));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	انجامبده
	{
		store[i] = (نویسه)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} وقتی(ch != c);

	بازگشت store;
}

ساختار Token* Tokenize_سطر(ساختار Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	اگر((35 == c) || (59 == c))
	{
		purge_سطر_Comment();
		بازگشت Tokenize_سطر(head);
	}

	اگر((9 == c) || (10 == c) || (32 == c))
	{
		بازگشت Tokenize_سطر(head);
	}

	ساختار Token* p = newToken();
	اگر(-1 == c)
	{
		Reached_EOF = true;
		آزاد(p);
		بازگشت head;
	}
	وگرنه اگر((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	وگرنه
	{
		p->Text = store_atom(c);
	}

	بازگشت addToken(head, p);
}

تهی setExpression(ساختار Token* p, نویسه match[], نویسه Exp[])
{
	/* Leave macros alone */
	اگر((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		بازگشت;
	}

	/* Only if there is an exact match replace */
	اگر(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	اگر(پوچ != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

تهی identify_macros(ساختار Token* p)
{
	اگر(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		اگر(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		وگرنه
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	اگر(پوچ != p->next)
	{
		identify_macros(p->next);
	}
}

تهی سطر_macro(ساختار Token* p)
{
	اگر(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	اگر(پوچ != p->next)
	{
		سطر_macro(p->next);
	}
}

تهی hexify_string(ساختار Token* p)
{
	نویسه table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	صحیح i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	نویسه* d = calloc(max_string, اندازه(نویسه));
	p->Expression = d;

	وقتی(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	وقتی( i < max_string)
	{
		اگر(0 == p->Text[i+1])
		{
			i = max_string;
		}
		وگرنه
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

تهی process_string(ساختار Token* p)
{
	اگر(p->type & str)
	{
		اگر('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		وگرنه اگر('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	اگر(پوچ != p->next)
	{
		process_string(p->next);
	}
}


تهی preserve_other(ساختار Token* p)
{
	اگر(پوچ != p->next)
	{
		preserve_other(p->next);
	}

	اگر((پوچ == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(نویسه a[])
{
	نویسه *ptr;
	بازگشت (uint16_t)strtol(a, &ptr, 0);
}

تهی eval_immediates(ساختار Token* p)
{
	اگر(پوچ != p->next)
	{
		eval_immediates(p->next);
	}

	اگر((پوچ == p->Expression) && !(p->type & macro))
	{
		uint16_t مقدار;
		مقدار = numerate_string(p->Text);

		اگر(('0' == p->Text[0]) || (0 != مقدار))
		{
			نویسه* c = calloc(5, اندازه(نویسه));
			sprintf(c, "%04x", مقدار);
			p->Expression = c;
		}
	}
}

تهی print_hex(ساختار Token* p)
{
	اگر(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	اگر(پوچ != p->next)
	{
		print_hex(p->next);
	}
	وگرنه
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
صحیح اصلی(صحیح argc, نویسه **argv)
{
	/* Make sure we have a program tape to run */
	اگر (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		بازگشت EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	ساختار Token* head = پوچ;
	وقتی(!Reached_EOF)
	{
		head = Tokenize_سطر(head);
	}

	identify_macros(head);
	سطر_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	بازگشت EXIT_SUCCESS;
}
