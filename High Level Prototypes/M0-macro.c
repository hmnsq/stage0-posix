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

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

संरचना Token
{
	संरचना Token* next;
	uint8_t type;
	वर्ण* Text;
	वर्ण* Expression;
};

गणना type
{
	macro = 1,
	str = (1 << 1)
};

संरचना Token* newToken()
{
	संरचना Token* p;

	p = calloc (1, परिमाण (संरचना Token));
	जइ (रिक्त == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	प्रत्यागम p;
}

संरचना Token* addToken(संरचना Token* head, संरचना Token* p)
{
	जइ(रिक्त == head)
	{
		प्रत्यागम p;
	}
	जइ(रिक्त == head->next)
	{
		head->next = p;
	}
	अण्णहा
	{
		addToken(head->next, p);
	}
	प्रत्यागम head;
}

शून्य purge_पङ्क्तिः_Comment()
{
	पूर्णाङ्क c = fgetc(source_file);
	जाव((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

वर्ण* store_atom(वर्ण c)
{
	वर्ण* store = calloc(max_string + 1, परिमाण(वर्ण));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	करेहि
	{
		store[i] = (वर्ण)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} जाव ((9 != ch) && (10 != ch) && (32 != ch));

	प्रत्यागम store;
}

वर्ण* store_string(वर्ण c)
{
	वर्ण* store = calloc(max_string + 1, परिमाण(वर्ण));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	करेहि
	{
		store[i] = (वर्ण)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} जाव(ch != c);

	प्रत्यागम store;
}

संरचना Token* Tokenize_पङ्क्तिः(संरचना Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	जइ((35 == c) || (59 == c))
	{
		purge_पङ्क्तिः_Comment();
		प्रत्यागम Tokenize_पङ्क्तिः(head);
	}

	जइ((9 == c) || (10 == c) || (32 == c))
	{
		प्रत्यागम Tokenize_पङ्क्तिः(head);
	}

	संरचना Token* p = newToken();
	जइ(-1 == c)
	{
		Reached_EOF = true;
		मुक्त(p);
		प्रत्यागम head;
	}
	अण्णहा जइ((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	अण्णहा
	{
		p->Text = store_atom(c);
	}

	प्रत्यागम addToken(head, p);
}

शून्य setExpression(संरचना Token* p, वर्ण match[], वर्ण Exp[])
{
	/* Leave macros alone */
	जइ((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		प्रत्यागम;
	}

	/* Only if there is an exact match replace */
	जइ(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	जइ(रिक्त != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

शून्य identify_macros(संरचना Token* p)
{
	जइ(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		जइ(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		अण्णहा
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	जइ(रिक्त != p->next)
	{
		identify_macros(p->next);
	}
}

शून्य पङ्क्तिः_macro(संरचना Token* p)
{
	जइ(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	जइ(रिक्त != p->next)
	{
		पङ्क्तिः_macro(p->next);
	}
}

शून्य hexify_string(संरचना Token* p)
{
	वर्ण table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	पूर्णाङ्क i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	वर्ण* d = calloc(max_string, परिमाण(वर्ण));
	p->Expression = d;

	जाव(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	जाव( i < max_string)
	{
		जइ(0 == p->Text[i+1])
		{
			i = max_string;
		}
		अण्णहा
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

शून्य process_string(संरचना Token* p)
{
	जइ(p->type & str)
	{
		जइ('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		अण्णहा जइ('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	जइ(रिक्त != p->next)
	{
		process_string(p->next);
	}
}


शून्य preserve_other(संरचना Token* p)
{
	जइ(रिक्त != p->next)
	{
		preserve_other(p->next);
	}

	जइ((रिक्त == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(वर्ण a[])
{
	वर्ण *ptr;
	प्रत्यागम (uint16_t)strtol(a, &ptr, 0);
}

शून्य eval_immediates(संरचना Token* p)
{
	जइ(रिक्त != p->next)
	{
		eval_immediates(p->next);
	}

	जइ((रिक्त == p->Expression) && !(p->type & macro))
	{
		uint16_t मानम्;
		मानम् = numerate_string(p->Text);

		जइ(('0' == p->Text[0]) || (0 != मानम्))
		{
			वर्ण* c = calloc(5, परिमाण(वर्ण));
			sprintf(c, "%04x", मानम्);
			p->Expression = c;
		}
	}
}

शून्य print_hex(संरचना Token* p)
{
	जइ(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	जइ(रिक्त != p->next)
	{
		print_hex(p->next);
	}
	अण्णहा
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv)
{
	/* Make sure we have a program tape to run */
	जइ (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		प्रत्यागम EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	संरचना Token* head = रिक्त;
	जाव(!Reached_EOF)
	{
		head = Tokenize_पङ्क्तिः(head);
	}

	identify_macros(head);
	पङ्क्तिः_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	प्रत्यागम EXIT_SUCCESS;
}
