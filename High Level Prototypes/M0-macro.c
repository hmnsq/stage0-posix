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

खाता* source_file;
bool Reached_EOF;

संरचना Token
{
	संरचना Token* next;
	uint8_t type;
	अक्षर* Text;
	अक्षर* Expression;
};

गणना type
{
	macro = 1,
	str = (1 << 1)
};

संरचना Token* newToken()
{
	संरचना Token* p;

	p = सुस्मृति (1, आकार (संरचना Token));
	जर (रिक्त == p)
	{
		ख_लिखो (मानक_त्रुटि, "calloc failed.\n");
		निकास (निकास_त्रुटि);
	}

	परत p;
}

संरचना Token* addToken(संरचना Token* head, संरचना Token* p)
{
	जर(रिक्त == head)
	{
		परत p;
	}
	जर(रिक्त == head->next)
	{
		head->next = p;
	}
	अन्यथा
	{
		addToken(head->next, p);
	}
	परत head;
}

शून्य purge_ओळ_Comment()
{
	पूर्णांक c = ख_अक्षर_लो(source_file);
	पर्यंत((10 != c) && (13 != c))
	{
		c = ख_अक्षर_लो(source_file);
	}
}

अक्षर* store_atom(अक्षर c)
{
	अक्षर* store = सुस्मृति(max_string + 1, आकार(अक्षर));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	करा
	{
		store[i] = (अक्षर)ch;
		ch = ख_अक्षर_लो(source_file);
		i = i + 1;
	} पर्यंत ((9 != ch) && (10 != ch) && (32 != ch));

	परत store;
}

अक्षर* store_string(अक्षर c)
{
	अक्षर* store = सुस्मृति(max_string + 1, आकार(अक्षर));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	करा
	{
		store[i] = (अक्षर)ch;
		i = i + 1;
		ch = ख_अक्षर_लो(source_file);
	} पर्यंत(ch != c);

	परत store;
}

संरचना Token* Tokenize_ओळ(संरचना Token* head)
{

	int32_t c;
	c = ख_अक्षर_लो(source_file);

	जर((35 == c) || (59 == c))
	{
		purge_ओळ_Comment();
		परत Tokenize_ओळ(head);
	}

	जर((9 == c) || (10 == c) || (32 == c))
	{
		परत Tokenize_ओळ(head);
	}

	संरचना Token* p = newToken();
	जर(-1 == c)
	{
		Reached_EOF = true;
		मुक्त(p);
		परत head;
	}
	अन्यथा जर((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	अन्यथा
	{
		p->Text = store_atom(c);
	}

	परत addToken(head, p);
}

शून्य setExpression(संरचना Token* p, अक्षर match[], अक्षर Exp[])
{
	/* Leave macros alone */
	जर((p->type & macro))
	{
		setExpression(p->next, match, Exp);
		परत;
	}

	/* Only if there is an exact match replace */
	जर(0 == म_भेदन(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	जर(रिक्त != p->next)
	{
		setExpression(p->next, match, Exp);
	}

}

शून्य identify_macros(संरचना Token* p)
{
	जर(0 == म_भेदन(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->next->Text;
		जर(p->next->next->type & str)
		{
			p->Expression = p->next->next->Text + 1;
		}
		अन्यथा
		{
			p->Expression = p->next->next->Text;
		}
		p->next = p->next->next->next;
	}

	जर(रिक्त != p->next)
	{
		identify_macros(p->next);
	}
}

शून्य ओळ_macro(संरचना Token* p)
{
	जर(p->type & macro)
	{
		setExpression(p->next, p->Text, p->Expression);
	}

	जर(रिक्त != p->next)
	{
		ओळ_macro(p->next);
	}
}

शून्य hexify_string(संरचना Token* p)
{
	अक्षर table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	पूर्णांक i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	अक्षर* d = सुस्मृति(max_string, आकार(अक्षर));
	p->Expression = d;

	पर्यंत(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	पर्यंत( i < max_string)
	{
		जर(0 == p->Text[i+1])
		{
			i = max_string;
		}
		अन्यथा
		{
			d[2*i]  = table[p->Text[i+1] / 16];
			d[2*i + 1] = table[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

शून्य process_string(संरचना Token* p)
{
	जर(p->type & str)
	{
		जर('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		अन्यथा जर('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	जर(रिक्त != p->next)
	{
		process_string(p->next);
	}
}


शून्य preserve_other(संरचना Token* p)
{
	जर(रिक्त != p->next)
	{
		preserve_other(p->next);
	}

	जर((रिक्त == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(अक्षर a[])
{
	अक्षर *ptr;
	परत (uint16_t)म_से_दीर्घ(a, &ptr, 0);
}

शून्य eval_immediates(संरचना Token* p)
{
	जर(रिक्त != p->next)
	{
		eval_immediates(p->next);
	}

	जर((रिक्त == p->Expression) && !(p->type & macro))
	{
		uint16_t मूल्य;
		मूल्य = numerate_string(p->Text);

		जर(('0' == p->Text[0]) || (0 != मूल्य))
		{
			अक्षर* c = सुस्मृति(5, आकार(अक्षर));
			प्र_लिखो(c, "%04x", मूल्य);
			p->Expression = c;
		}
	}
}

शून्य print_hex(संरचना Token* p)
{
	जर(p->type ^ macro)
	{
		ख_लिखो(मानक_निकास, "\n%s", p->Expression);
	}

	जर(रिक्त != p->next)
	{
		print_hex(p->next);
	}
	अन्यथा
	{
		ख_लिखो(मानक_निकास, "\n");
	}
}

/* Standard C main program */
पूर्णांक main(पूर्णांक argc, अक्षर **argv)
{
	/* Make sure we have a program tape to run */
	जर (argc < 2)
	{
		ख_लिखो(मानक_त्रुटि, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		परत निकास_त्रुटि;
	}

	source_file = ख_खोलो(argv[1], "r");

	Reached_EOF = false;
	संरचना Token* head = रिक्त;
	पर्यंत(!Reached_EOF)
	{
		head = Tokenize_ओळ(head);
	}

	identify_macros(head);
	ओळ_macro(head);
	process_string(head);
	eval_immediates(head);
	preserve_other(head);
	print_hex(head);

	परत निकास_सफल;
}
