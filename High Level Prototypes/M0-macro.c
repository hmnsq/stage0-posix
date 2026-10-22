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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

sa_nracanaa Token
{
	sa_nracanaa Token* next;
	uint8_t type;
	var_na* Text;
	var_na* Expression;
};

ga_nanaa type
{
	macro = 1,
	str = (1 << 1)
};

sa_nracanaa Token* newToken()
{
	sa_nracanaa Token* p;

	p = calloc (1, parimaa_na (sa_nracanaa Token));
	ᬬᬾᬦ᭄ (NULL == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	ᬯᬮᬶ p;
}

sa_nracanaa Token* addToken(sa_nracanaa Token* head, sa_nracanaa Token* p)
{
	ᬬᬾᬦ᭄(NULL == head)
	{
		ᬯᬮᬶ p;
	}
	ᬬᬾᬦ᭄(NULL == head->next)
	{
		head->next = p;
	}
	ᬮᬶᬬ
	{
		addToken(head->next, p);
	}
	ᬯᬮᬶ head;
}

void purge_lineComment()
{
	puur_naa_nka c = fgetc(source_file);
	ᬲᭂᬤᭂᬮ((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

var_na* store_atom(var_na c)
{
	var_na* store = calloc(max_string + 1, parimaa_na(var_na));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	do
	{
		store[i] = (var_na)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} ᬲᭂᬤᭂᬮ ((9 != ch) && (10 != ch) && (32 != ch));

	ᬯᬮᬶ store;
}

var_na* store_string(var_na c)
{
	var_na* store = calloc(max_string + 1, parimaa_na(var_na));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	do
	{
		store[i] = (var_na)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} ᬲᭂᬤᭂᬮ(ch != c);

	ᬯᬮᬶ store;
}

sa_nracanaa Token* Tokenize_Line(sa_nracanaa Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	ᬬᬾᬦ᭄((35 == c) || (59 == c))
	{
		purge_lineComment();
		ᬯᬮᬶ Tokenize_Line(head);
	}

	ᬬᬾᬦ᭄((9 == c) || (10 == c) || (32 == c))
	{
		ᬯᬮᬶ Tokenize_Line(head);
	}

	sa_nracanaa Token* p = newToken();
	ᬬᬾᬦ᭄(-1 == c)
	{
		Reached_EOF = true;
		free(p);
		ᬯᬮᬶ head;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	ᬮᬶᬬ
	{
		p->Text = store_atom(c);
	}

	ᬯᬮᬶ addToken(head, p);
}

void setExpression(sa_nracanaa Token* p, var_na match[], var_na Exp[])
{
	/* Leave macros alone */
	ᬬᬾᬦ᭄((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		ᬯᬮᬶ;
	}

	/* Only if there is an exact match replace */
	ᬬᬾᬦ᭄(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	ᬬᬾᬦ᭄(NULL != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

void identify_macros(sa_nracanaa Token* p)
{
	ᬬᬾᬦ᭄(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		ᬬᬾᬦ᭄(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		ᬮᬶᬬ
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	ᬬᬾᬦ᭄(NULL != p->next)
	{
		identify_macros(p->next);
	}
}

void line_macro(sa_nracanaa Token* p)
{
	ᬬᬾᬦ᭄(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	ᬬᬾᬦ᭄(NULL != p->next)
	{
		line_macro(p->next);
	}
}

void hexify_string(sa_nracanaa Token* p)
{
	var_na table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	puur_naa_nka i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	var_na* d = calloc(max_string, parimaa_na(var_na));
	p->Expression = d;

	ᬲᭂᬤᭂᬮ(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	ᬲᭂᬤᭂᬮ( i < max_string)
	{
		ᬬᬾᬦ᭄(0 == p->Text[i+1])
		{
			i = max_string;
		}
		ᬮᬶᬬ
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

void process_string(sa_nracanaa Token* p)
{
	ᬬᬾᬦ᭄(p->type & str)
	{
		ᬬᬾᬦ᭄('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		ᬮᬶᬬ ᬬᬾᬦ᭄('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	ᬬᬾᬦ᭄(NULL != p->next)
	{
		process_string(p->next);
	}
}


void preserve_other(sa_nracanaa Token* p)
{
	ᬬᬾᬦ᭄(NULL != p->next)
	{
		preserve_other(p->next);
	}

	ᬬᬾᬦ᭄((NULL == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(var_na a[])
{
	var_na *ptr;
	ᬯᬮᬶ (uint16_t)strtol(a, &ptr, 0);
}

void eval_immediates(sa_nracanaa Token* p)
{
	ᬬᬾᬦ᭄(NULL != p->next)
	{
		eval_immediates(p->next);
	}

	ᬬᬾᬦ᭄((NULL == p->Expression) && !(p->type & macro))
	{
		uint16_t value;
		value = numerate_string(p->Text);

		ᬬᬾᬦ᭄(('0' == p->Text[0]) || (0 != value))
		{
			var_na* c = calloc(5, parimaa_na(var_na));
			sprintf(c, "%04x", value);
			p->Expression = c;
		}
	}
}

void print_hex(sa_nracanaa Token* p)
{
	ᬬᬾᬦ᭄(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	ᬬᬾᬦ᭄(NULL != p->next)
	{
		print_hex(p->next);
	}
	ᬮᬶᬬ
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
puur_naa_nka main(puur_naa_nka argc, var_na **argv)
{
	/* Make sure we have a program tape to run */
	ᬬᬾᬦ᭄ (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		ᬯᬮᬶ EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	sa_nracanaa Token* head = NULL;
	ᬲᭂᬤᭂᬮ(!Reached_EOF)
	{
		head = Tokenize_Line(head);
	}

	identify_macros(head);
	line_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	ᬯᬮᬶ EXIT_SUCCESS;
}
