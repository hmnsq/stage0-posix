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

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <stdbool.h>
#અન્તર્ભાવ <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

સંરચના Token
{
	સંરચના Token* next;
	uint8_t type;
	અક્ષર* Text;
	અક્ષર* Expression;
};

ગણના type
{
	macro = 1,
	str = (1 << 1)
};

સંરચના Token* newToken()
{
	સંરચના Token* p;

	p = calloc (1, કદ (સંરચના Token));
	જો (ખાલી == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	પાછા p;
}

સંરચના Token* addToken(સંરચના Token* head, સંરચના Token* p)
{
	જો(ખાલી == head)
	{
		પાછા p;
	}
	જો(ખાલી == head->next)
	{
		head->next = p;
	}
	નહીંતર
	{
		addToken(head->next, p);
	}
	પાછા head;
}

શૂન્ય purge_લીટી_Comment()
{
	પૂર્ણાંક c = fgetc(source_file);
	જ્યારે((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

અક્ષર* store_atom(અક્ષર c)
{
	અક્ષર* store = calloc(max_string + 1, કદ(અક્ષર));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	કરો
	{
		store[i] = (અક્ષર)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} જ્યારે ((9 != ch) && (10 != ch) && (32 != ch));

	પાછા store;
}

અક્ષર* store_string(અક્ષર c)
{
	અક્ષર* store = calloc(max_string + 1, કદ(અક્ષર));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	કરો
	{
		store[i] = (અક્ષર)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} જ્યારે(ch != c);

	પાછા store;
}

સંરચના Token* Tokenize_લીટી(સંરચના Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	જો((35 == c) || (59 == c))
	{
		purge_લીટી_Comment();
		પાછા Tokenize_લીટી(head);
	}

	જો((9 == c) || (10 == c) || (32 == c))
	{
		પાછા Tokenize_લીટી(head);
	}

	સંરચના Token* p = newToken();
	જો(-1 == c)
	{
		Reached_EOF = true;
		મુક્ત(p);
		પાછા head;
	}
	નહીંતર જો((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	નહીંતર
	{
		p->Text = store_atom(c);
	}

	પાછા addToken(head, p);
}

શૂન્ય setExpression(સંરચના Token* p, અક્ષર match[], અક્ષર Exp[])
{
	/* Leave macros alone */
	જો((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		પાછા;
	}

	/* Only if there is an exact match replace */
	જો(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	જો(ખાલી != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

શૂન્ય identify_macros(સંરચના Token* p)
{
	જો(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		જો(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		નહીંતર
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	જો(ખાલી != p->next)
	{
		identify_macros(p->next);
	}
}

શૂન્ય લીટી_macro(સંરચના Token* p)
{
	જો(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	જો(ખાલી != p->next)
	{
		લીટી_macro(p->next);
	}
}

શૂન્ય hexify_string(સંરચના Token* p)
{
	અક્ષર table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	પૂર્ણાંક i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	અક્ષર* d = calloc(max_string, કદ(અક્ષર));
	p->Expression = d;

	જ્યારે(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	જ્યારે( i < max_string)
	{
		જો(0 == p->Text[i+1])
		{
			i = max_string;
		}
		નહીંતર
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

શૂન્ય process_string(સંરચના Token* p)
{
	જો(p->type & str)
	{
		જો('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		નહીંતર જો('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	જો(ખાલી != p->next)
	{
		process_string(p->next);
	}
}


શૂન્ય preserve_other(સંરચના Token* p)
{
	જો(ખાલી != p->next)
	{
		preserve_other(p->next);
	}

	જો((ખાલી == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(અક્ષર a[])
{
	અક્ષર *ptr;
	પાછા (uint16_t)strtol(a, &ptr, 0);
}

શૂન્ય eval_immediates(સંરચના Token* p)
{
	જો(ખાલી != p->next)
	{
		eval_immediates(p->next);
	}

	જો((ખાલી == p->Expression) && !(p->type & macro))
	{
		uint16_t કિંમત;
		કિંમત = numerate_string(p->Text);

		જો(('0' == p->Text[0]) || (0 != કિંમત))
		{
			અક્ષર* c = calloc(5, કદ(અક્ષર));
			sprintf(c, "%04x", કિંમત);
			p->Expression = c;
		}
	}
}

શૂન્ય print_hex(સંરચના Token* p)
{
	જો(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	જો(ખાલી != p->next)
	{
		print_hex(p->next);
	}
	નહીંતર
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
	/* Make sure we have a program tape to run */
	જો (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		પાછા EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	સંરચના Token* head = ખાલી;
	જ્યારે(!Reached_EOF)
	{
		head = Tokenize_લીટી(head);
	}

	identify_macros(head);
	લીટી_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	પાછા EXIT_SUCCESS;
}
