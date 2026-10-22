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
	만약 (NULL == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	반환 p;
}

struct Token* addToken(struct Token* head, struct Token* p)
{
	만약(NULL == head)
	{
		반환 p;
	}
	만약(NULL == head->next)
	{
		head->next = p;
	}
	아니면
	{
		addToken(head->next, p);
	}
	반환 head;
}

void purge_lineComment()
{
	정수 c = fgetc(source_file);
	동안((10 != c) && (13 != c))
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
	} 동안 ((9 != ch) && (10 != ch) && (32 != ch));

	반환 store;
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
	} 동안(ch != c);

	반환 store;
}

struct Token* Tokenize_Line(struct Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	만약((35 == c) || (59 == c))
	{
		purge_lineComment();
		반환 Tokenize_Line(head);
	}

	만약((9 == c) || (10 == c) || (32 == c))
	{
		반환 Tokenize_Line(head);
	}

	struct Token* p = newToken();
	만약(-1 == c)
	{
		Reached_EOF = true;
		free(p);
		반환 head;
	}
	아니면 만약((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	아니면
	{
		p->Text = store_atom(c);
	}

	반환 addToken(head, p);
}

void setExpression(struct Token* p, char match[], char Exp[])
{
	/* Leave macros alone */
	만약((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		반환;
	}

	/* Only if there is an exact match replace */
	만약(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	만약(NULL != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

void identify_macros(struct Token* p)
{
	만약(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		만약(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		아니면
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	만약(NULL != p->next)
	{
		identify_macros(p->next);
	}
}

void line_macro(struct Token* p)
{
	만약(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	만약(NULL != p->next)
	{
		line_macro(p->next);
	}
}

void hexify_string(struct Token* p)
{
	char table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	정수 i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	char* d = calloc(max_string, sizeof(char));
	p->Expression = d;

	동안(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	동안( i < max_string)
	{
		만약(0 == p->Text[i+1])
		{
			i = max_string;
		}
		아니면
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

void process_string(struct Token* p)
{
	만약(p->type & str)
	{
		만약('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		아니면 만약('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	만약(NULL != p->next)
	{
		process_string(p->next);
	}
}


void preserve_other(struct Token* p)
{
	만약(NULL != p->next)
	{
		preserve_other(p->next);
	}

	만약((NULL == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(char a[])
{
	char *ptr;
	반환 (uint16_t)strtol(a, &ptr, 0);
}

void eval_immediates(struct Token* p)
{
	만약(NULL != p->next)
	{
		eval_immediates(p->next);
	}

	만약((NULL == p->Expression) && !(p->type & macro))
	{
		uint16_t value;
		value = numerate_string(p->Text);

		만약(('0' == p->Text[0]) || (0 != value))
		{
			char* c = calloc(5, sizeof(char));
			sprintf(c, "%04x", value);
			p->Expression = c;
		}
	}
}

void print_hex(struct Token* p)
{
	만약(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	만약(NULL != p->next)
	{
		print_hex(p->next);
	}
	아니면
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
정수 main(정수 argc, char **argv)
{
	/* Make sure we have a program tape to run */
	만약 (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		반환 EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	struct Token* head = NULL;
	동안(!Reached_EOF)
	{
		head = Tokenize_Line(head);
	}

	identify_macros(head);
	line_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	반환 EXIT_SUCCESS;
}
