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
	אם (عدم == p)
	{
		fprintf (stderr, "calloc failed.\n");
		exit (EXIT_FAILURE);
	}

	החזר p;
}

بنية Token* addToken(بنية Token* head, بنية Token* p)
{
	אם(عدم == head)
	{
		החזר p;
	}
	אם(عدم == head->next)
	{
		head->next = p;
	}
	אחרת
	{
		addToken(head->next, p);
	}
	החזר head;
}

فراغ purge_lineComment()
{
	صحيح c = fgetc(source_file);
	כלעוד((10 != c) && (13 != c))
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
	עשה
	{
		store[i] = (حرف)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} כלעוד ((9 != ch) && (10 != ch) && (32 != ch));

	החזר store;
}

حرف* store_string(حرف c)
{
	حرف* store = calloc(max_string + 1, حجم(حرف));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	עשה
	{
		store[i] = (حرف)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} כלעוד(ch != c);

	החזר store;
}

بنية Token* Tokenize_Line(بنية Token* head)
{

	int32_t c;
	c = fgetc(source_file);

	אם((35 == c) || (59 == c))
	{
		purge_lineComment();
		החזר Tokenize_Line(head);
	}

	אם((9 == c) || (10 == c) || (32 == c))
	{
		החזר Tokenize_Line(head);
	}

	بنية Token* p = newToken();
	אם(-1 == c)
	{
		Reached_EOF = true;
		حرر(p);
		החזר head;
	}
	אחרת אם((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	אחרת
	{
		p->Text = store_atom(c);
	}

	החזר addToken(head, p);
}

فراغ setExpression(بنية Token* p, حرف match[], حرف Exp[])
{
	/* Leave macros alone */
	אם((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		החזר;
	}

	/* Only if there is an exact match replace */
	אם(0 == strncmp(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	אם(عدم != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

فراغ identify_macros(بنية Token* p)
{
	אם(0 == strncmp(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		אם(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		אחרת
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	אם(عدم != p->next)
	{
		identify_macros(p->next);
	}
}

فراغ line_macro(بنية Token* p)
{
	אם(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	אם(عدم != p->next)
	{
		line_macro(p->next);
	}
}

فراغ hexify_string(بنية Token* p)
{
	حرف table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	صحيح i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	حرف* d = calloc(max_string, حجم(حرف));
	p->Expression = d;

	כלעוד(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	כלעוד( i < max_string)
	{
		אם(0 == p->Text[i+1])
		{
			i = max_string;
		}
		אחרת
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

فراغ process_string(بنية Token* p)
{
	אם(p->type & str)
	{
		אם('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		אחרת אם('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	אם(عدم != p->next)
	{
		process_string(p->next);
	}
}


فراغ preserve_other(بنية Token* p)
{
	אם(عدم != p->next)
	{
		preserve_other(p->next);
	}

	אם((عدم == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(حرف a[])
{
	حرف *ptr;
	החזר (uint16_t)strtol(a, &ptr, 0);
}

فراغ eval_immediates(بنية Token* p)
{
	אם(عدم != p->next)
	{
		eval_immediates(p->next);
	}

	אם((عدم == p->Expression) && !(p->type & macro))
	{
		uint16_t value;
		value = numerate_string(p->Text);

		אם(('0' == p->Text[0]) || (0 != value))
		{
			حرف* c = calloc(5, حجم(حرف));
			sprintf(c, "%04x", value);
			p->Expression = c;
		}
	}
}

فراغ print_hex(بنية Token* p)
{
	אם(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	אם(عدم != p->next)
	{
		print_hex(p->next);
	}
	אחרת
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
صحيح main(صحيح argc, حرف **argv)
{
	/* Make sure we have a program tape to run */
	אם (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		החזר EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	بنية Token* head = عدم;
	כלעוד(!Reached_EOF)
	{
		head = Tokenize_Line(head);
	}

	identify_macros(head);
	line_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	החזר EXIT_SUCCESS;
}
