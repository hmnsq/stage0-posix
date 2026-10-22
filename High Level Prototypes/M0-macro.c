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

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <stdint.h>
#ضمن <stdbool.h>
#ضمن <string.h>
#define max_string 63

FILE* source_file;
bool Reached_EOF;

بنية Token
{
	بنية Token* next;
	uint8_t type;
	حرف* Text;
	حرف* Expression;
};

تعداد type
{
	macro = 1,
	str = (1 << 1)
};

بنية Token* newToken()
{
	بنية Token* p;

	p = calloc (1, حجم (بنية Token));
	إذا (عدم == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	أرجع p;
}

بنية Token* addToken(بنية Token* head, بنية Token* p)
{
	إذا(عدم == head)
	{
		أرجع p;
	}
	إذا(عدم == head->next)
	{
		head->next = p;
	}
	وإلا
	{
		addToken(head->next, p);
	}
	أرجع head;
}

فراغ purge_باريس_Comment()
{
	صحيح c = fgetc(source_file);
	طالما((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

حرف* store_atom(حرف c)
{
	حرف* store = calloc(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} طالما ((9 != ch) && (10 != ch) && (32 != ch));

	أرجع store;
}

حرف* store_string(حرف c)
{
	حرف* store = calloc(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	افعل
	{
		store[i] = (حرف)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} طالما(ch != c);

	أرجع store;
}

بنية Token* Tokenize_باريس(بنية Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	إذا((35 == c) || (59 == c))
	{
		purge_باريس_Comment();
		أرجع Tokenize_باريس(head);
	}

	إذا((9 == c) || (10 == c) || (32 == c))
	{
		أرجع Tokenize_باريس(head);
	}

	بنية Token* p = newToken();
	إذا(-1 == c)
	{
		Reached_EOF = true;
		حرر(p);
		أرجع head;
	}
	وإلا إذا((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	وإلا
	{
		p->Text = store_atom(c);
	}

	أرجع addToken(head, p);
}

فراغ setExpression(بنية Token* p, حرف match[], حرف Exp[])
{
	/* Leave macros alone */
	إذا((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		أرجع;
	}

	/* Only if there is an exact match replace */
	إذا(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	إذا(عدم != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

فراغ identify_macros(بنية Token* p)
{
	إذا(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		إذا(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		وإلا
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	إذا(عدم != p->next)
	{
		identify_macros(p->next);
	}
}

فراغ باريس_macro(بنية Token* p)
{
	إذا(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	إذا(عدم != p->next)
	{
		باريس_macro(p->next);
	}
}

فراغ hexify_string(بنية Token* p)
{
	حرف table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	صحيح i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	حرف* d = calloc(max_string, حجم(حرف));
	p->Expression = d;

	طالما(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	طالما( i < max_string)
	{
		إذا(0 == p->Text[i+1])
		{
			i = max_string;
		}
		وإلا
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

فراغ process_string(بنية Token* p)
{
	إذا(p->type & str)
	{
		إذا('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		وإلا إذا('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	إذا(عدم != p->next)
	{
		process_string(p->next);
	}
}


فراغ preserve_other(بنية Token* p)
{
	إذا(عدم != p->next)
	{
		preserve_other(p->next);
	}

	إذا((عدم == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(حرف a[])
{
	حرف *ptr;
	أرجع (uint16_t)strtol(a, &ptr, 0);
}

فراغ eval_immediates(بنية Token* p)
{
	إذا(عدم != p->next)
	{
		eval_immediates(p->next);
	}

	إذا((عدم == p->Expression) && !(p->type & macro))
	{
		uint16_t نيلاي;
		نيلاي = numerate_string(p->Text);

		إذا(('0' == p->Text[0]) || (0 != نيلاي))
		{
			حرف* c = calloc(5, حجم(حرف));
			sprintf(c, "%04x", نيلاي);
			p->Expression = c;
		}
	}
}

فراغ print_hex(بنية Token* p)
{
	إذا(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	إذا(عدم != p->next)
	{
		print_hex(p->next);
	}
	وإلا
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
صحيح main(صحيح argc, حرف **argv)
{
	/* Make sure we have a program tape to run */
	إذا (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		أرجع EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	بنية Token* head = عدم;
	طالما(!Reached_EOF)
	{
		head = Tokenize_باريس(head);
	}

	identify_macros(head);
	باريس_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	أرجع EXIT_SUCCESS;
}
