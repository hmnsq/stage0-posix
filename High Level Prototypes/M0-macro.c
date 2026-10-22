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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <stdbool.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

ସଂରଚନା Token
{
	ସଂରଚନା Token* next;
	uint8_t type;
	ଅକ୍ଷର* Text;
	ଅକ୍ଷର* Expression;
};

ଗଣନା type
{
	macro = 1,
	str = (1 << 1)
};

ସଂରଚନା Token* newToken()
{
	ସଂରଚନା Token* p;

	p = calloc (1, ଆକାର (ସଂରଚନା Token));
	ଯଦି (ଖାଲି == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	ଫେରନ୍ତୁ p;
}

ସଂରଚନା Token* addToken(ସଂରଚନା Token* head, ସଂରଚନା Token* p)
{
	ଯଦି(ଖାଲି == head)
	{
		ଫେରନ୍ତୁ p;
	}
	ଯଦି(ଖାଲି == head->next)
	{
		head->next = p;
	}
	ଅନ୍ୟଥା
	{
		addToken(head->next, p);
	}
	ଫେରନ୍ତୁ head;
}

ଶୂନ୍ୟ purge_ଧାଡ଼ି_Comment()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = fgetc(source_file);
	ଯେତେବେଳେ((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

ଅକ୍ଷର* store_atom(ଅକ୍ଷର c)
{
	ଅକ୍ଷର* store = calloc(max_string + 1, ଆକାର(ଅକ୍ଷର));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	କର
	{
		store[i] = (ଅକ୍ଷର)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} ଯେତେବେଳେ ((9 != ch) && (10 != ch) && (32 != ch));

	ଫେରନ୍ତୁ store;
}

ଅକ୍ଷର* store_string(ଅକ୍ଷର c)
{
	ଅକ୍ଷର* store = calloc(max_string + 1, ଆକାର(ଅକ୍ଷର));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	କର
	{
		store[i] = (ଅକ୍ଷର)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} ଯେତେବେଳେ(ch != c);

	ଫେରନ୍ତୁ store;
}

ସଂରଚନା Token* Tokenize_ଧାଡ଼ି(ସଂରଚନା Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	ଯଦି((35 == c) || (59 == c))
	{
		purge_ଧାଡ଼ି_Comment();
		ଫେରନ୍ତୁ Tokenize_ଧାଡ଼ି(head);
	}

	ଯଦି((9 == c) || (10 == c) || (32 == c))
	{
		ଫେରନ୍ତୁ Tokenize_ଧାଡ଼ି(head);
	}

	ସଂରଚନା Token* p = newToken();
	ଯଦି(-1 == c)
	{
		Reached_EOF = true;
		ମୁକ୍ତ(p);
		ଫେରନ୍ତୁ head;
	}
	ଅନ୍ୟଥା ଯଦି((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	ଅନ୍ୟଥା
	{
		p->Text = store_atom(c);
	}

	ଫେରନ୍ତୁ addToken(head, p);
}

ଶୂନ୍ୟ setExpression(ସଂରଚନା Token* p, ଅକ୍ଷର match[], ଅକ୍ଷର Exp[])
{
	/* Leave macros alone */
	ଯଦି((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		ଫେରନ୍ତୁ;
	}

	/* Only if there is an exact match replace */
	ଯଦି(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	ଯଦି(ଖାଲି != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

ଶୂନ୍ୟ identify_macros(ସଂରଚନା Token* p)
{
	ଯଦି(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		ଯଦି(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		ଅନ୍ୟଥା
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	ଯଦି(ଖାଲି != p->next)
	{
		identify_macros(p->next);
	}
}

ଶୂନ୍ୟ ଧାଡ଼ି_macro(ସଂରଚନା Token* p)
{
	ଯଦି(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	ଯଦି(ଖାଲି != p->next)
	{
		ଧାଡ଼ି_macro(p->next);
	}
}

ଶୂନ୍ୟ hexify_string(ସଂରଚନା Token* p)
{
	ଅକ୍ଷର table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	ଅକ୍ଷର* d = calloc(max_string, ଆକାର(ଅକ୍ଷର));
	p->Expression = d;

	ଯେତେବେଳେ(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	ଯେତେବେଳେ( i < max_string)
	{
		ଯଦି(0 == p->Text[i+1])
		{
			i = max_string;
		}
		ଅନ୍ୟଥା
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

ଶୂନ୍ୟ process_string(ସଂରଚନା Token* p)
{
	ଯଦି(p->type & str)
	{
		ଯଦି('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		ଅନ୍ୟଥା ଯଦି('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	ଯଦି(ଖାଲି != p->next)
	{
		process_string(p->next);
	}
}


ଶୂନ୍ୟ preserve_other(ସଂରଚନା Token* p)
{
	ଯଦି(ଖାଲି != p->next)
	{
		preserve_other(p->next);
	}

	ଯଦି((ଖାଲି == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(ଅକ୍ଷର a[])
{
	ଅକ୍ଷର *ptr;
	ଫେରନ୍ତୁ (uint16_t)strtol(a, &ptr, 0);
}

ଶୂନ୍ୟ eval_immediates(ସଂରଚନା Token* p)
{
	ଯଦି(ଖାଲି != p->next)
	{
		eval_immediates(p->next);
	}

	ଯଦି((ଖାଲି == p->Expression) && !(p->type & macro))
	{
		uint16_t ମୂଲ୍ୟ;
		ମୂଲ୍ୟ = numerate_string(p->Text);

		ଯଦି(('0' == p->Text[0]) || (0 != ମୂଲ୍ୟ))
		{
			ଅକ୍ଷର* c = calloc(5, ଆକାର(ଅକ୍ଷର));
			sprintf(c, "%04x", ମୂଲ୍ୟ);
			p->Expression = c;
		}
	}
}

ଶୂନ୍ୟ print_hex(ସଂରଚନା Token* p)
{
	ଯଦି(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	ଯଦି(ଖାଲି != p->next)
	{
		print_hex(p->next);
	}
	ଅନ୍ୟଥା
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
	/* Make sure we have a program tape to run */
	ଯଦି (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		ଫେରନ୍ତୁ EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	ସଂରଚନା Token* head = ଖାଲି;
	ଯେତେବେଳେ(!Reached_EOF)
	{
		head = Tokenize_ଧାଡ଼ି(head);
	}

	identify_macros(head);
	ଧାଡ଼ି_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	ଫେରନ୍ତୁ EXIT_SUCCESS;
}
