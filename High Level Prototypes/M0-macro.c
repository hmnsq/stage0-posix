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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

ಸಂರಚನಾ Token
{
	ಸಂರಚನಾ Token* next;
	uint8_t type;
	ವರ್ಣ* Text;
	ವರ್ಣ* Expression;
};

ಗಣನಾ type
{
	macro = 1,
	str = (1 << 1)
};

ಸಂರಚನಾ Token* newToken()
{
	ಸಂರಚನಾ Token* p;

	p = calloc (1, ಪರಿಮಾಣ (ಸಂರಚನಾ Token));
	ಯದಿ (ರಿಕ್ತ == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	ಪ್ರತ್ಯಾಗಮ p;
}

ಸಂರಚನಾ Token* addToken(ಸಂರಚನಾ Token* head, ಸಂರಚನಾ Token* p)
{
	ಯದಿ(ರಿಕ್ತ == head)
	{
		ಪ್ರತ್ಯಾಗಮ p;
	}
	ಯದಿ(ರಿಕ್ತ == head->next)
	{
		head->next = p;
	}
	ಅನ್ಯಥಾ
	{
		addToken(head->next, p);
	}
	ಪ್ರತ್ಯಾಗಮ head;
}

ಶೂನ್ಯ purge_ಸಾಲು_Comment()
{
	ಪೂರ್ಣಾಙ್ಕ c = fgetc(source_file);
	ಯಾವತ್((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

ವರ್ಣ* store_atom(ವರ್ಣ c)
{
	ವರ್ಣ* store = calloc(max_string + 1, ಪರಿಮಾಣ(ವರ್ಣ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ಕುರು
	{
		store[i] = (ವರ್ಣ)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} ಯಾವತ್ ((9 != ch) && (10 != ch) && (32 != ch));

	ಪ್ರತ್ಯಾಗಮ store;
}

ವರ್ಣ* store_string(ವರ್ಣ c)
{
	ವರ್ಣ* store = calloc(max_string + 1, ಪರಿಮಾಣ(ವರ್ಣ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ಕುರು
	{
		store[i] = (ವರ್ಣ)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} ಯಾವತ್(ch != c);

	ಪ್ರತ್ಯಾಗಮ store;
}

ಸಂರಚನಾ Token* Tokenize_ಸಾಲು(ಸಂರಚನಾ Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	ಯದಿ((35 == c) || (59 == c))
	{
		purge_ಸಾಲು_Comment();
		ಪ್ರತ್ಯಾಗಮ Tokenize_ಸಾಲು(head);
	}

	ಯದಿ((9 == c) || (10 == c) || (32 == c))
	{
		ಪ್ರತ್ಯಾಗಮ Tokenize_ಸಾಲು(head);
	}

	ಸಂರಚನಾ Token* p = newToken();
	ಯದಿ(-1 == c)
	{
		Reached_EOF = true;
		ಮುಕ್ತ(p);
		ಪ್ರತ್ಯಾಗಮ head;
	}
	ಅನ್ಯಥಾ ಯದಿ((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	ಅನ್ಯಥಾ
	{
		p->Text = store_atom(c);
	}

	ಪ್ರತ್ಯಾಗಮ addToken(head, p);
}

ಶೂನ್ಯ setExpression(ಸಂರಚನಾ Token* p, ವರ್ಣ match[], ವರ್ಣ Exp[])
{
	/* Leave macros alone */
	ಯದಿ((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		ಪ್ರತ್ಯಾಗಮ;
	}

	/* Only if there is an exact match replace */
	ಯದಿ(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	ಯದಿ(ರಿಕ್ತ != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

ಶೂನ್ಯ identify_macros(ಸಂರಚನಾ Token* p)
{
	ಯದಿ(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		ಯದಿ(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		ಅನ್ಯಥಾ
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	ಯದಿ(ರಿಕ್ತ != p->next)
	{
		identify_macros(p->next);
	}
}

ಶೂನ್ಯ ಸಾಲು_macro(ಸಂರಚನಾ Token* p)
{
	ಯದಿ(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	ಯದಿ(ರಿಕ್ತ != p->next)
	{
		ಸಾಲು_macro(p->next);
	}
}

ಶೂನ್ಯ hexify_string(ಸಂರಚನಾ Token* p)
{
	ವರ್ಣ table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	ಪೂರ್ಣಾಙ್ಕ i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	ವರ್ಣ* d = calloc(max_string, ಪರಿಮಾಣ(ವರ್ಣ));
	p->Expression = d;

	ಯಾವತ್(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	ಯಾವತ್( i < max_string)
	{
		ಯದಿ(0 == p->Text[i+1])
		{
			i = max_string;
		}
		ಅನ್ಯಥಾ
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

ಶೂನ್ಯ process_string(ಸಂರಚನಾ Token* p)
{
	ಯದಿ(p->type & str)
	{
		ಯದಿ('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		ಅನ್ಯಥಾ ಯದಿ('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	ಯದಿ(ರಿಕ್ತ != p->next)
	{
		process_string(p->next);
	}
}


ಶೂನ್ಯ preserve_other(ಸಂರಚನಾ Token* p)
{
	ಯದಿ(ರಿಕ್ತ != p->next)
	{
		preserve_other(p->next);
	}

	ಯದಿ((ರಿಕ್ತ == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(ವರ್ಣ a[])
{
	ವರ್ಣ *ptr;
	ಪ್ರತ್ಯಾಗಮ (uint16_t)strtol(a, &ptr, 0);
}

ಶೂನ್ಯ eval_immediates(ಸಂರಚನಾ Token* p)
{
	ಯದಿ(ರಿಕ್ತ != p->next)
	{
		eval_immediates(p->next);
	}

	ಯದಿ((ರಿಕ್ತ == p->Expression) && !(p->type & macro))
	{
		uint16_t ಮೌಲ್ಯ;
		ಮೌಲ್ಯ = numerate_string(p->Text);

		ಯದಿ(('0' == p->Text[0]) || (0 != ಮೌಲ್ಯ))
		{
			ವರ್ಣ* c = calloc(5, ಪರಿಮಾಣ(ವರ್ಣ));
			sprintf(c, "%04x", ಮೌಲ್ಯ);
			p->Expression = c;
		}
	}
}

ಶೂನ್ಯ print_hex(ಸಂರಚನಾ Token* p)
{
	ಯದಿ(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	ಯದಿ(ರಿಕ್ತ != p->next)
	{
		print_hex(p->next);
	}
	ಅನ್ಯಥಾ
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
	/* Make sure we have a program tape to run */
	ಯದಿ (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		ಪ್ರತ್ಯಾಗಮ EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	ಸಂರಚನಾ Token* head = ರಿಕ್ತ;
	ಯಾವತ್(!Reached_EOF)
	{
		head = Tokenize_ಸಾಲು(head);
	}

	identify_macros(head);
	ಸಾಲು_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	ಪ್ರತ್ಯಾಗಮ EXIT_SUCCESS;
}
