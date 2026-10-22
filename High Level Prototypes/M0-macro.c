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
	නම් (NULL == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	ආපසු p;
}

sa_nracanaa Token* addToken(sa_nracanaa Token* head, sa_nracanaa Token* p)
{
	නම්(NULL == head)
	{
		ආපසු p;
	}
	නම්(NULL == head->next)
	{
		head->next = p;
	}
	නැතහොත්
	{
		addToken(head->next, p);
	}
	ආපසු head;
}

void purge_lineComment()
{
	පූර්ණ_සංඛ්‍යා c = fgetc(source_file);
	අතරතුර((10 != c) && (13 != c))
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
	} අතරතුර ((9 != ch) && (10 != ch) && (32 != ch));

	ආපසු store;
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
	} අතරතුර(ch != c);

	ආපසු store;
}

sa_nracanaa Token* Tokenize_Line(sa_nracanaa Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	නම්((35 == c) || (59 == c))
	{
		purge_lineComment();
		ආපසු Tokenize_Line(head);
	}

	නම්((9 == c) || (10 == c) || (32 == c))
	{
		ආපසු Tokenize_Line(head);
	}

	sa_nracanaa Token* p = newToken();
	නම්(-1 == c)
	{
		Reached_EOF = true;
		free(p);
		ආපසු head;
	}
	නැතහොත් නම්((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	නැතහොත්
	{
		p->Text = store_atom(c);
	}

	ආපසු addToken(head, p);
}

void setExpression(sa_nracanaa Token* p, var_na match[], var_na Exp[])
{
	/* Leave macros alone */
	නම්((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		ආපසු;
	}

	/* Only if there is an exact match replace */
	නම්(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	නම්(NULL != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

void identify_macros(sa_nracanaa Token* p)
{
	නම්(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		නම්(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		නැතහොත්
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	නම්(NULL != p->next)
	{
		identify_macros(p->next);
	}
}

void line_macro(sa_nracanaa Token* p)
{
	නම්(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	නම්(NULL != p->next)
	{
		line_macro(p->next);
	}
}

void hexify_string(sa_nracanaa Token* p)
{
	var_na table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	පූර්ණ_සංඛ්‍යා i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	var_na* d = calloc(max_string, parimaa_na(var_na));
	p->Expression = d;

	අතරතුර(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	අතරතුර( i < max_string)
	{
		නම්(0 == p->Text[i+1])
		{
			i = max_string;
		}
		නැතහොත්
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

void process_string(sa_nracanaa Token* p)
{
	නම්(p->type & str)
	{
		නම්('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		නැතහොත් නම්('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	නම්(NULL != p->next)
	{
		process_string(p->next);
	}
}


void preserve_other(sa_nracanaa Token* p)
{
	නම්(NULL != p->next)
	{
		preserve_other(p->next);
	}

	නම්((NULL == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(var_na a[])
{
	var_na *ptr;
	ආපසු (uint16_t)strtol(a, &ptr, 0);
}

void eval_immediates(sa_nracanaa Token* p)
{
	නම්(NULL != p->next)
	{
		eval_immediates(p->next);
	}

	නම්((NULL == p->Expression) && !(p->type & macro))
	{
		uint16_t value;
		value = numerate_string(p->Text);

		නම්(('0' == p->Text[0]) || (0 != value))
		{
			var_na* c = calloc(5, parimaa_na(var_na));
			sprintf(c, "%04x", value);
			p->Expression = c;
		}
	}
}

void print_hex(sa_nracanaa Token* p)
{
	නම්(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	නම්(NULL != p->next)
	{
		print_hex(p->next);
	}
	නැතහොත්
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
පූර්ණ_සංඛ්‍යා main(පූර්ණ_සංඛ්‍යා argc, var_na **argv)
{
	/* Make sure we have a program tape to run */
	නම් (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		ආපසු EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	sa_nracanaa Token* head = NULL;
	අතරතුර(!Reached_EOF)
	{
		head = Tokenize_Line(head);
	}

	identify_macros(head);
	line_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	ආපසු EXIT_SUCCESS;
}
