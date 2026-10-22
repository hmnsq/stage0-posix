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

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <stdbool.h>
#അന്തര്ഭാവ <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

ഘടന Token
{
	ഘടന Token* next;
	uint8_t type;
	അക്ഷരം* Text;
	അക്ഷരം* Expression;
};

എണ്ണൽ type
{
	macro = 1,
	str = (1 << 1)
};

ഘടന Token* newToken()
{
	ഘടന Token* p;

	p = calloc (1, വലുപ്പം (ഘടന Token));
	എങ്കിൽ (ശൂന്യ == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	തിരികെ p;
}

ഘടന Token* addToken(ഘടന Token* head, ഘടന Token* p)
{
	എങ്കിൽ(ശൂന്യ == head)
	{
		തിരികെ p;
	}
	എങ്കിൽ(ശൂന്യ == head->next)
	{
		head->next = p;
	}
	അല്ലെങ്കിൽ
	{
		addToken(head->next, p);
	}
	തിരികെ head;
}

ശൂന്യം purge_വരി_Comment()
{
	പൂർണ്ണസംഖ്യ c = fgetc(source_file);
	ആകുമ്പോൾ((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

അക്ഷരം* store_atom(അക്ഷരം c)
{
	അക്ഷരം* store = calloc(max_string + 1, വലുപ്പം(അക്ഷരം));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ചെയ്യുക
	{
		store[i] = (അക്ഷരം)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} ആകുമ്പോൾ ((9 != ch) && (10 != ch) && (32 != ch));

	തിരികെ store;
}

അക്ഷരം* store_string(അക്ഷരം c)
{
	അക്ഷരം* store = calloc(max_string + 1, വലുപ്പം(അക്ഷരം));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ചെയ്യുക
	{
		store[i] = (അക്ഷരം)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} ആകുമ്പോൾ(ch != c);

	തിരികെ store;
}

ഘടന Token* Tokenize_വരി(ഘടന Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	എങ്കിൽ((35 == c) || (59 == c))
	{
		purge_വരി_Comment();
		തിരികെ Tokenize_വരി(head);
	}

	എങ്കിൽ((9 == c) || (10 == c) || (32 == c))
	{
		തിരികെ Tokenize_വരി(head);
	}

	ഘടന Token* p = newToken();
	എങ്കിൽ(-1 == c)
	{
		Reached_EOF = true;
		സ്വതന്ത്രം(p);
		തിരികെ head;
	}
	അല്ലെങ്കിൽ എങ്കിൽ((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	അല്ലെങ്കിൽ
	{
		p->Text = store_atom(c);
	}

	തിരികെ addToken(head, p);
}

ശൂന്യം setExpression(ഘടന Token* p, അക്ഷരം match[], അക്ഷരം Exp[])
{
	/* Leave macros alone */
	എങ്കിൽ((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		തിരികെ;
	}

	/* Only if there is an exact match replace */
	എങ്കിൽ(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	എങ്കിൽ(ശൂന്യ != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

ശൂന്യം identify_macros(ഘടന Token* p)
{
	എങ്കിൽ(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		എങ്കിൽ(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		അല്ലെങ്കിൽ
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	എങ്കിൽ(ശൂന്യ != p->next)
	{
		identify_macros(p->next);
	}
}

ശൂന്യം വരി_macro(ഘടന Token* p)
{
	എങ്കിൽ(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	എങ്കിൽ(ശൂന്യ != p->next)
	{
		വരി_macro(p->next);
	}
}

ശൂന്യം hexify_string(ഘടന Token* p)
{
	അക്ഷരം table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	പൂർണ്ണസംഖ്യ i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	അക്ഷരം* d = calloc(max_string, വലുപ്പം(അക്ഷരം));
	p->Expression = d;

	ആകുമ്പോൾ(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	ആകുമ്പോൾ( i < max_string)
	{
		എങ്കിൽ(0 == p->Text[i+1])
		{
			i = max_string;
		}
		അല്ലെങ്കിൽ
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

ശൂന്യം process_string(ഘടന Token* p)
{
	എങ്കിൽ(p->type & str)
	{
		എങ്കിൽ('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		അല്ലെങ്കിൽ എങ്കിൽ('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	എങ്കിൽ(ശൂന്യ != p->next)
	{
		process_string(p->next);
	}
}


ശൂന്യം preserve_other(ഘടന Token* p)
{
	എങ്കിൽ(ശൂന്യ != p->next)
	{
		preserve_other(p->next);
	}

	എങ്കിൽ((ശൂന്യ == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(അക്ഷരം a[])
{
	അക്ഷരം *ptr;
	തിരികെ (uint16_t)strtol(a, &ptr, 0);
}

ശൂന്യം eval_immediates(ഘടന Token* p)
{
	എങ്കിൽ(ശൂന്യ != p->next)
	{
		eval_immediates(p->next);
	}

	എങ്കിൽ((ശൂന്യ == p->Expression) && !(p->type & macro))
	{
		uint16_t മൂല്യം;
		മൂല്യം = numerate_string(p->Text);

		എങ്കിൽ(('0' == p->Text[0]) || (0 != മൂല്യം))
		{
			അക്ഷരം* c = calloc(5, വലുപ്പം(അക്ഷരം));
			sprintf(c, "%04x", മൂല്യം);
			p->Expression = c;
		}
	}
}

ശൂന്യം print_hex(ഘടന Token* p)
{
	എങ്കിൽ(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	എങ്കിൽ(ശൂന്യ != p->next)
	{
		print_hex(p->next);
	}
	അല്ലെങ്കിൽ
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
	/* Make sure we have a program tape to run */
	എങ്കിൽ (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		തിരികെ EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	ഘടന Token* head = ശൂന്യ;
	ആകുമ്പോൾ(!Reached_EOF)
	{
		head = Tokenize_വരി(head);
	}

	identify_macros(head);
	വരി_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	തിരികെ EXIT_SUCCESS;
}
