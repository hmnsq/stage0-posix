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

#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <stdint.h>
#समावेश <stdbool.h>
#समावेश <string.h>
#घोषणा max_string 63

खाता* source_file;
bool Reached_EOF;

काष्ठा Token
{
	काष्ठा Token* अग्र;
	uint8_t type;
	अक्षर* Text;
	अक्षर* Expression;
};

क्रमागत type
{
	macro = 1,
	str = (1 << 1)
};

काष्ठा Token* newToken()
{
	काष्ठा Token* p;

	p = सुस्मृति (1, माप (काष्ठा Token));
	अगर (शून्य == p)
	{
		ख_लिखो (मानक_त्रुटि, "calloc failed.\n");
		निकास (निकास_त्रुटि);
	}

	वापस p;
}

काष्ठा Token* addToken(काष्ठा Token* शीर्ष, काष्ठा Token* p)
{
	अगर(शून्य == शीर्ष)
	{
		वापस p;
	}
	अगर(शून्य == शीर्ष->अग्र)
	{
		शीर्ष->अग्र = p;
	}
	अन्यथा
	{
		addToken(शीर्ष->अग्र, p);
	}
	वापस शीर्ष;
}

व्योम purge_पंक्ति_Comment()
{
	पूर्णांक c = ख_अक्षर_लो(source_file);
	जबतक((10 != c) && (13 != c))
	{
		c = ख_अक्षर_लो(source_file);
	}
}

अक्षर* store_atom(अक्षर c)
{
	अक्षर* store = सुस्मृति(max_string + 1, माप(अक्षर));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	करो
	{
		store[i] = (अक्षर)ch;
		ch = ख_अक्षर_लो(source_file);
		i = i + 1;
	} जबतक ((9 != ch) && (10 != ch) && (32 != ch));

	वापस store;
}

अक्षर* store_string(अक्षर c)
{
	अक्षर* store = सुस्मृति(max_string + 1, माप(अक्षर));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	करो
	{
		store[i] = (अक्षर)ch;
		i = i + 1;
		ch = ख_अक्षर_लो(source_file);
	} जबतक(ch != c);

	वापस store;
}

काष्ठा Token* Tokenize_पंक्ति(काष्ठा Token* शीर्ष)
{

	int32_t c;
	c = ख_अक्षर_लो(source_file);

	अगर((35 == c) || (59 == c))
	{
		purge_पंक्ति_Comment();
		वापस Tokenize_पंक्ति(शीर्ष);
	}

	अगर((9 == c) || (10 == c) || (32 == c))
	{
		वापस Tokenize_पंक्ति(शीर्ष);
	}

	काष्ठा Token* p = newToken();
	अगर(-1 == c)
	{
		Reached_EOF = true;
		मुक्त(p);
		वापस शीर्ष;
	}
	अन्यथा अगर((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	अन्यथा
	{
		p->Text = store_atom(c);
	}

	वापस addToken(शीर्ष, p);
}

व्योम setExpression(काष्ठा Token* p, अक्षर match[], अक्षर Exp[])
{
	/* Leave macros alone */
	अगर((p->type & macro))
	{
		setExpression(p->अग्र, match, Exp);
		वापस;
	}

	/* Only if there is an exact match replace */
	अगर(0 == म_भेदन(p->Text, match, max_string))
	{
		p->Expression = Exp;
	}

	अगर(शून्य != p->अग्र)
	{
		setExpression(p->अग्र, match, Exp);
	}

}

व्योम identify_macros(काष्ठा Token* p)
{
	अगर(0 == म_भेदन(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->अग्र->Text;
		अगर(p->अग्र->अग्र->type & str)
		{
			p->Expression = p->अग्र->अग्र->Text + 1;
		}
		अन्यथा
		{
			p->Expression = p->अग्र->अग्र->Text;
		}
		p->अग्र = p->अग्र->अग्र->अग्र;
	}

	अगर(शून्य != p->अग्र)
	{
		identify_macros(p->अग्र);
	}
}

व्योम पंक्ति_macro(काष्ठा Token* p)
{
	अगर(p->type & macro)
	{
		setExpression(p->अग्र, p->Text, p->Expression);
	}

	अगर(शून्य != p->अग्र)
	{
		पंक्ति_macro(p->अग्र);
	}
}

व्योम hexify_string(काष्ठा Token* p)
{
	अक्षर table[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	पूर्णांक i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	अक्षर* d = सुस्मृति(max_string, माप(अक्षर));
	p->Expression = d;

	जबतक(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	जबतक( i < max_string)
	{
		अगर(0 == p->Text[i+1])
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

व्योम process_string(काष्ठा Token* p)
{
	अगर(p->type & str)
	{
		अगर('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		अन्यथा अगर('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	अगर(शून्य != p->अग्र)
	{
		process_string(p->अग्र);
	}
}


व्योम preserve_other(काष्ठा Token* p)
{
	अगर(शून्य != p->अग्र)
	{
		preserve_other(p->अग्र);
	}

	अगर((शून्य == p->Expression) && !(p->type & macro))
	{
		p->Expression = p->Text;
	}
}

uint16_t numerate_string(अक्षर a[])
{
	अक्षर *ptr;
	वापस (uint16_t)म_से_दीर्घ(a, &ptr, 0);
}

व्योम eval_immediates(काष्ठा Token* p)
{
	अगर(शून्य != p->अग्र)
	{
		eval_immediates(p->अग्र);
	}

	अगर((शून्य == p->Expression) && !(p->type & macro))
	{
		uint16_t मूल्य;
		मूल्य = numerate_string(p->Text);

		अगर(('0' == p->Text[0]) || (0 != मूल्य))
		{
			अक्षर* c = सुस्मृति(5, माप(अक्षर));
			प्र_लिखो(c, "%04x", मूल्य);
			p->Expression = c;
		}
	}
}

व्योम print_hex(काष्ठा Token* p)
{
	अगर(p->type ^ macro)
	{
		ख_लिखो(मानक_निकास, "\n%s", p->Expression);
	}

	अगर(शून्य != p->अग्र)
	{
		print_hex(p->अग्र);
	}
	अन्यथा
	{
		ख_लिखो(मानक_निकास, "\n");
	}
}

/* Standard C main program */
पूर्णांक मुख्य(पूर्णांक argc, अक्षर **argv)
{
	/* Make sure we have a program tape to run */
	अगर (argc < 2)
	{
		ख_लिखो(मानक_त्रुटि, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		वापस निकास_त्रुटि;
	}

	source_file = ख_खोलो(argv[1], "r");

	Reached_EOF = false;
	काष्ठा Token* शीर्ष = शून्य;
	जबतक(!Reached_EOF)
	{
		शीर्ष = Tokenize_पंक्ति(शीर्ष);
	}

	identify_macros(शीर्ष);
	पंक्ति_macro(शीर्ष);
	process_string(शीर्ष);
	eval_immediates(शीर्ष);
	preserve_other(शीर्ष);
	print_hex(शीर्ष);

	वापस निकास_सफल;
}
