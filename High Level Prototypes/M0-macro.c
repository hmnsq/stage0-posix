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

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <stdbool.h>
#அந்தர்பாவ <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

கட்டமைப்பு Token
{
	கட்டமைப்பு Token* next;
	uint8_t type;
	எழுத்து* Text;
	எழுத்து* Expression;
};

எண்ணிக்கை type
{
	macro = 1,
	str = (1 << 1)
};

கட்டமைப்பு Token* newToken()
{
	கட்டமைப்பு Token* p;

	p = calloc (1, அளவு (கட்டமைப்பு Token));
	எனில் (வெற்று == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	திரும்பு p;
}

கட்டமைப்பு Token* addToken(கட்டமைப்பு Token* head, கட்டமைப்பு Token* p)
{
	எனில்(வெற்று == head)
	{
		திரும்பு p;
	}
	எனில்(வெற்று == head->next)
	{
		head->next = p;
	}
	இல்லையெனில்
	{
		addToken(head->next, p);
	}
	திரும்பு head;
}

வெறுமை purge_வரி_Comment()
{
	முழு c = fgetc(source_file);
	போது((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

எழுத்து* store_atom(எழுத்து c)
{
	எழுத்து* store = calloc(max_string + 1, அளவு(எழுத்து));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	செய்
	{
		store[i] = (எழுத்து)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} போது ((9 != ch) && (10 != ch) && (32 != ch));

	திரும்பு store;
}

எழுத்து* store_string(எழுத்து c)
{
	எழுத்து* store = calloc(max_string + 1, அளவு(எழுத்து));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	செய்
	{
		store[i] = (எழுத்து)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} போது(ch != c);

	திரும்பு store;
}

கட்டமைப்பு Token* Tokenize_வரி(கட்டமைப்பு Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	எனில்((35 == c) || (59 == c))
	{
		purge_வரி_Comment();
		திரும்பு Tokenize_வரி(head);
	}

	எனில்((9 == c) || (10 == c) || (32 == c))
	{
		திரும்பு Tokenize_வரி(head);
	}

	கட்டமைப்பு Token* p = newToken();
	எனில்(-1 == c)
	{
		Reached_EOF = true;
		விடுவி(p);
		திரும்பு head;
	}
	இல்லையெனில் எனில்((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	இல்லையெனில்
	{
		p->Text = store_atom(c);
	}

	திரும்பு addToken(head, p);
}

வெறுமை setExpression(கட்டமைப்பு Token* p, எழுத்து match[], எழுத்து Exp[])
{
	/* Leave macros alone */
	எனில்((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		திரும்பு;
	}

	/* Only if there is an exact match replace */
	எனில்(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	எனில்(வெற்று != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

வெறுமை identify_macros(கட்டமைப்பு Token* p)
{
	எனில்(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		எனில்(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		இல்லையெனில்
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	எனில்(வெற்று != p->next)
	{
		identify_macros(p->next);
	}
}

வெறுமை வரி_macro(கட்டமைப்பு Token* p)
{
	எனில்(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	எனில்(வெற்று != p->next)
	{
		வரி_macro(p->next);
	}
}

வெறுமை hexify_string(கட்டமைப்பு Token* p)
{
	எழுத்து table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	முழு i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	எழுத்து* d = calloc(max_string, அளவு(எழுத்து));
	p->Expression = d;

	போது(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	போது( i < max_string)
	{
		எனில்(0 == p->Text[i+1])
		{
			i = max_string;
		}
		இல்லையெனில்
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

வெறுமை process_string(கட்டமைப்பு Token* p)
{
	எனில்(p->type & str)
	{
		எனில்('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		இல்லையெனில் எனில்('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	எனில்(வெற்று != p->next)
	{
		process_string(p->next);
	}
}


வெறுமை preserve_other(கட்டமைப்பு Token* p)
{
	எனில்(வெற்று != p->next)
	{
		preserve_other(p->next);
	}

	எனில்((வெற்று == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(எழுத்து a[])
{
	எழுத்து *ptr;
	திரும்பு (uint16_t)strtol(a, &ptr, 0);
}

வெறுமை eval_immediates(கட்டமைப்பு Token* p)
{
	எனில்(வெற்று != p->next)
	{
		eval_immediates(p->next);
	}

	எனில்((வெற்று == p->Expression) && !(p->type & macro))
	{
		uint16_t மதிப்பு;
		மதிப்பு = numerate_string(p->Text);

		எனில்(('0' == p->Text[0]) || (0 != மதிப்பு))
		{
			எழுத்து* c = calloc(5, அளவு(எழுத்து));
			sprintf(c, "%04x", மதிப்பு);
			p->Expression = c;
		}
	}
}

வெறுமை print_hex(கட்டமைப்பு Token* p)
{
	எனில்(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	எனில்(வெற்று != p->next)
	{
		print_hex(p->next);
	}
	இல்லையெனில்
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
முழு முதன்மை(முழு argc, எழுத்து **argv)
{
	/* Make sure we have a program tape to run */
	எனில் (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		திரும்பு EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	கட்டமைப்பு Token* head = வெற்று;
	போது(!Reached_EOF)
	{
		head = Tokenize_வரி(head);
	}

	identify_macros(head);
	வரி_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	திரும்பு EXIT_SUCCESS;
}
