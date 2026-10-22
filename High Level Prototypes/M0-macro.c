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

struct Token
{
	struct Token* next;
	uint8_t type;
	char* Text;
	char* Expression;
};

enum type
{
	macro = 1,
	str = (1 << 1)
};

struct Token* newToken()
{
	struct Token* p;

	p = calloc (1, sizeof (struct Token));
	якщо (NULL == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	повернути p;
}

struct Token* addToken(struct Token* head, struct Token* p)
{
	якщо(NULL == head)
	{
		повернути p;
	}
	якщо(NULL == head->next)
	{
		head->next = p;
	}
	інакше
	{
		addToken(head->next, p);
	}
	повернути head;
}

void purge_lineComment()
{
	ціле c = fgetc(source_file);
	поки((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

char* store_atom(char c)
{
	char* store = calloc(max_string + 1, sizeof(char));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	do
	{
		store[i] = (char)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} поки ((9 != ch) && (10 != ch) && (32 != ch));

	повернути store;
}

char* store_string(char c)
{
	char* store = calloc(max_string + 1, sizeof(char));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	do
	{
		store[i] = (char)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} поки(ch != c);

	повернути store;
}

struct Token* Tokenize_Line(struct Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	якщо((35 == c) || (59 == c))
	{
		purge_lineComment();
		повернути Tokenize_Line(head);
	}

	якщо((9 == c) || (10 == c) || (32 == c))
	{
		повернути Tokenize_Line(head);
	}

	struct Token* p = newToken();
	якщо(-1 == c)
	{
		Reached_EOF = true;
		free(p);
		повернути head;
	}
	інакше якщо((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	інакше
	{
		p->Text = store_atom(c);
	}

	повернути addToken(head, p);
}

void setExpression(struct Token* p, char match[], char Exp[])
{
	/* Leave macros alone */
	якщо((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		повернути;
	}

	/* Only if there is an exact match replace */
	якщо(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	якщо(NULL != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

void identify_macros(struct Token* p)
{
	якщо(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		якщо(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		інакше
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	якщо(NULL != p->next)
	{
		identify_macros(p->next);
	}
}

void line_macro(struct Token* p)
{
	якщо(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	якщо(NULL != p->next)
	{
		line_macro(p->next);
	}
}

void hexify_string(struct Token* p)
{
	char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	ціле i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	char* d = calloc(max_string, sizeof(char));
	p->Expression = d;

	поки(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	поки( i < max_string)
	{
		якщо(0 == p->Text[i+1])
		{
			i = max_string;
		}
		інакше
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

void process_string(struct Token* p)
{
	якщо(p->type & str)
	{
		якщо('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		інакше якщо('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	якщо(NULL != p->next)
	{
		process_string(p->next);
	}
}


void preserve_other(struct Token* p)
{
	якщо(NULL != p->next)
	{
		preserve_other(p->next);
	}

	якщо((NULL == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(char a[])
{
	char *ptr;
	повернути (uint16_t)strtol(a, &ptr, 0);
}

void eval_immediates(struct Token* p)
{
	якщо(NULL != p->next)
	{
		eval_immediates(p->next);
	}

	якщо((NULL == p->Expression) && !(p->type & macro))
	{
		uint16_t value;
		value = numerate_string(p->Text);

		якщо(('0' == p->Text[0]) || (0 != value))
		{
			char* c = calloc(5, sizeof(char));
			sprintf(c, "%04x", value);
			p->Expression = c;
		}
	}
}

void print_hex(struct Token* p)
{
	якщо(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	якщо(NULL != p->next)
	{
		print_hex(p->next);
	}
	інакше
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
ціле main(ціле argc, char **argv)
{
	/* Make sure we have a program tape to run */
	якщо (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		повернути EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	struct Token* head = NULL;
	поки(!Reached_EOF)
	{
		head = Tokenize_Line(head);
	}

	identify_macros(head);
	line_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	повернути EXIT_SUCCESS;
}
