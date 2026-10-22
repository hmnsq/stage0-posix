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
#परिभाषय max_string 63

FILE* source_file;
bool Reached_EOF;

संरचना Token
{
	संरचना Token* अग्र;
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

	p = शून्यस्मृतिं_देहि (1, परिमाण (संरचना Token));
	यदि (रिक्त == p)
	{
		fprintf (stderr, "calloc failed.\n");
		निर्गच्छ (EXIT_FAILURE);
	}

	प्रत्यागम p;
}

संरचना Token* सङ्कलय_Token(संरचना Token* शीर्ष, संरचना Token* p)
{
	यदि(रिक्त == शीर्ष)
	{
		प्रत्यागम p;
	}
	यदि(रिक्त == शीर्ष->अग्र)
	{
		शीर्ष->अग्र = p;
	}
	अन्यथा
	{
		सङ्कलय_Token(शीर्ष->अग्र, p);
	}
	प्रत्यागम शीर्ष;
}

शून्य purge_पङ्क्तिः_Comment()
{
	पूर्णाङ्क c = fgetc(source_file);
	यावत्((10 != c) && (13 != c))
	{
		c = fgetc(source_file);
	}
}

वर्ण* store_atom(वर्ण c)
{
	वर्ण* store = शून्यस्मृतिं_देहि(max_string + 1, परिमाण(वर्ण));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	कुरु
	{
		store[i] = (वर्ण)ch;
		ch = fgetc(source_file);
		i = i + 1;
	} यावत् ((9 != ch) && (10 != ch) && (32 != ch));

	प्रत्यागम store;
}

वर्ण* store_string(वर्ण c)
{
	वर्ण* store = शून्यस्मृतिं_देहि(max_string + 1, परिमाण(वर्ण));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	कुरु
	{
		store[i] = (वर्ण)ch;
		i = i + 1;
		ch = fgetc(source_file);
	} यावत्(ch != c);

	प्रत्यागम store;
}

संरचना Token* Tokenize_पङ्क्तिः(संरचना Token* शीर्ष)
{

	int32_t c;
	c = fgetc(source_file);

	यदि((35 == c) || (59 == c))
	{
		purge_पङ्क्तिः_Comment();
		प्रत्यागम Tokenize_पङ्क्तिः(शीर्ष);
	}

	यदि((9 == c) || (10 == c) || (32 == c))
	{
		प्रत्यागम Tokenize_पङ्क्तिः(शीर्ष);
	}

	संरचना Token* p = newToken();
	यदि(-1 == c)
	{
		Reached_EOF = true;
		मुक्त(p);
		प्रत्यागम शीर्ष;
	}
	अन्यथा यदि((34 == c) || (39 == c))
	{
		p->Text = store_string(c);
		p->type = str;
	}
	अन्यथा
	{
		p->Text = store_atom(c);
	}

	प्रत्यागम सङ्कलय_Token(शीर्ष, p);
}

शून्य समुच्चयः_Expression(संरचना Token* p, वर्ण मेलनम्[], वर्ण Exp[])
{
	/* Leave macros alone */
	यदि((p->type & macro))
	{
		समुच्चयः_Expression(p->अग्र, मेलनम्, Exp);
		प्रत्यागम;
	}

	/* Only if there is an exact match replace */
	यदि(0 == वाक्ये_तुलय(p->Text, मेलनम्, max_string))
	{
		p->Expression = Exp;
	}

	यदि(रिक्त != p->अग्र)
	{
		समुच्चयः_Expression(p->अग्र, मेलनम्, Exp);
	}

}

शून्य identify_macros(संरचना Token* p)
{
	यदि(0 == वाक्ये_तुलय(p->Text, "DEFINE", max_string))
	{
		p->type = macro;
		p->Text = p->अग्र->Text;
		यदि(p->अग्र->अग्र->type & str)
		{
			p->Expression = p->अग्र->अग्र->Text + 1;
		}
		अन्यथा
		{
			p->Expression = p->अग्र->अग्र->Text;
		}
		p->अग्र = p->अग्र->अग्र->अग्र;
	}

	यदि(रिक्त != p->अग्र)
	{
		identify_macros(p->अग्र);
	}
}

शून्य पङ्क्तिः_macro(संरचना Token* p)
{
	यदि(p->type & macro)
	{
		समुच्चयः_Expression(p->अग्र, p->Text, p->Expression);
	}

	यदि(रिक्त != p->अग्र)
	{
		पङ्क्तिः_macro(p->अग्र);
	}
}

शून्य hexify_string(संरचना Token* p)
{
	वर्ण सारणी[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	पूर्णाङ्क i = ((strnlen(p->Text + 1 , max_string)/4) + 1) * 8;

	वर्ण* d = शून्यस्मृतिं_देहि(max_string, परिमाण(वर्ण));
	p->Expression = d;

	यावत्(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	यावत्( i < max_string)
	{
		यदि(0 == p->Text[i+1])
		{
			i = max_string;
		}
		अन्यथा
		{
			d[2*i]  = सारणी[p->Text[i+1] / 16];
			d[2*i + 1] = सारणी[p->Text[i+1] % 16];
			i = i + 1;
		}
	}
}

शून्य process_string(संरचना Token* p)
{
	यदि(p->type & str)
	{
		यदि('\'' == p->Text[0])
		{
			p->Expression = p->Text + 1;
		}
		अन्यथा यदि('"' == p->Text[0])
		{
			hexify_string(p);
		}
	}

	यदि(रिक्त != p->अग्र)
	{
		process_string(p->अग्र);
	}
}


शून्य preserve_other(संरचना Token* p)
{
	यदि(रिक्त != p->अग्र)
	{
		preserve_other(p->अग्र);
	}

	यदि((रिक्त == p->Expression) && !(p->type & macro))
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
	यदि(रिक्त != p->अग्र)
	{
		eval_immediates(p->अग्र);
	}

	यदि((रिक्त == p->Expression) && !(p->type & macro))
	{
		uint16_t मानम्;
		मानम् = numerate_string(p->Text);

		यदि(('0' == p->Text[0]) || (0 != मानम्))
		{
			वर्ण* c = शून्यस्मृतिं_देहि(5, परिमाण(वर्ण));
			sprintf(c, "%04x", मानम्);
			p->Expression = c;
		}
	}
}

शून्य print_hex(संरचना Token* p)
{
	यदि(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->Expression);
	}

	यदि(रिक्त != p->अग्र)
	{
		print_hex(p->अग्र);
	}
	अन्यथा
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
पूर्णाङ्क मुख्यम्(पूर्णाङ्क argc, वर्ण **argv)
{
	/* Make sure we have a program tape to run */
	यदि (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		प्रत्यागम EXIT_FAILURE;
	}

	source_file = fopen(argv[1], "r");

	Reached_EOF = false;
	संरचना Token* शीर्ष = रिक्त;
	यावत्(!Reached_EOF)
	{
		शीर्ष = Tokenize_पङ्क्तिः(शीर्ष);
	}

	identify_macros(शीर्ष);
	पङ्क्तिः_macro(शीर्ष);
	process_string(शीर्ष);
	eval_immediates(शीर्ष);
	preserve_other(शीर्ष);
	print_hex(शीर्ष);

	प्रत्यागम EXIT_SUCCESS;
}
