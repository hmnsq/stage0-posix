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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <stdbool.h>
#అన్తర్భావ <string.h>
#నిర్వచించు max_string 63

FILE* ఆధారం_file;
bool Reached_EOF;

నిర్మాణం Token
{
	నిర్మాణం Token* తదుపరి;
	uint8_t type;
	అక్షరం* పాఠ్యం;
	అక్షరం* వ్యక్తీకరణ;
};

గణన type
{
	macro = 1,
	str = (1 << 1)
};

నిర్మాణం Token* newToken()
{
	నిర్మాణం Token* p;

	p = సున్నా_జ్ఞాపకం_ఇవ్వు (1, పరిమాణం (నిర్మాణం Token));
	ఒకవేళ (ఖాళీ == p)
	{
		fprintf (stderr, "calloc failed.\n");
		నిష్క్రమించు (EXIT_FAILURE);
	}

	తిరిగి p;
}

నిర్మాణం Token* కూడు_Token(నిర్మాణం Token* తల, నిర్మాణం Token* p)
{
	ఒకవేళ(ఖాళీ == తల)
	{
		తిరిగి p;
	}
	ఒకవేళ(ఖాళీ == తల->తదుపరి)
	{
		తల->తదుపరి = p;
	}
	లేకపోతే
	{
		కూడు_Token(తల->తదుపరి, p);
	}
	తిరిగి తల;
}

శూన్య purge_పంక్తి_Comment()
{
	పూర్ణాంకం c = fgetc(ఆధారం_file);
	అయితే((10 != c) && (13 != c))
	{
		c = fgetc(ఆధారం_file);
	}
}

అక్షరం* store_atom(అక్షరం c)
{
	అక్షరం* store = సున్నా_జ్ఞాపకం_ఇవ్వు(max_string + 1, పరిమాణం(అక్షరం));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	చేయి
	{
		store[i] = (అక్షరం)ch;
		ch = fgetc(ఆధారం_file);
		i = i + 1;
	} అయితే ((9 != ch) && (10 != ch) && (32 != ch));

	తిరిగి store;
}

అక్షరం* store_string(అక్షరం c)
{
	అక్షరం* store = సున్నా_జ్ఞాపకం_ఇవ్వు(max_string + 1, పరిమాణం(అక్షరం));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	చేయి
	{
		store[i] = (అక్షరం)ch;
		i = i + 1;
		ch = fgetc(ఆధారం_file);
	} అయితే(ch != c);

	తిరిగి store;
}

నిర్మాణం Token* Tokenize_పంక్తి(నిర్మాణం Token* తల)
{

	int32_t c;
	c = fgetc(ఆధారం_file);

	ఒకవేళ((35 == c) || (59 == c))
	{
		purge_పంక్తి_Comment();
		తిరిగి Tokenize_పంక్తి(తల);
	}

	ఒకవేళ((9 == c) || (10 == c) || (32 == c))
	{
		తిరిగి Tokenize_పంక్తి(తల);
	}

	నిర్మాణం Token* p = newToken();
	ఒకవేళ(-1 == c)
	{
		Reached_EOF = true;
		విడుదల(p);
		తిరిగి తల;
	}
	లేకపోతే ఒకవేళ((34 == c) || (39 == c))
	{
		p->పాఠ్యం = store_string(c);
		p->type = str;
	}
	లేకపోతే
	{
		p->పాఠ్యం = store_atom(c);
	}

	తిరిగి కూడు_Token(తల, p);
}

శూన్య సమితి_వ్యక్తీకరణ(నిర్మాణం Token* p, అక్షరం సరిపోలిక[], అక్షరం Exp[])
{
	/* Leave macros alone */
	ఒకవేళ((p->type & macro))
	{
		సమితి_వ్యక్తీకరణ(p->తదుపరి, సరిపోలిక, Exp);
		తిరిగి;
	}

	/* Only if there is an exact match replace */
	ఒకవేళ(0 == అక్షరశ్రేణి_పోలిక(p->పాఠ్యం, సరిపోలిక, max_string))
	{
		p->వ్యక్తీకరణ = Exp;
	}

	ఒకవేళ(ఖాళీ != p->తదుపరి)
	{
		సమితి_వ్యక్తీకరణ(p->తదుపరి, సరిపోలిక, Exp);
	}

}

శూన్య identify_macros(నిర్మాణం Token* p)
{
	ఒకవేళ(0 == అక్షరశ్రేణి_పోలిక(p->పాఠ్యం, "DEFINE", max_string))
	{
		p->type = macro;
		p->పాఠ్యం = p->తదుపరి->పాఠ్యం;
		ఒకవేళ(p->తదుపరి->తదుపరి->type & str)
		{
			p->వ్యక్తీకరణ = p->తదుపరి->తదుపరి->పాఠ్యం + 1;
		}
		లేకపోతే
		{
			p->వ్యక్తీకరణ = p->తదుపరి->తదుపరి->పాఠ్యం;
		}
		p->తదుపరి = p->తదుపరి->తదుపరి->తదుపరి;
	}

	ఒకవేళ(ఖాళీ != p->తదుపరి)
	{
		identify_macros(p->తదుపరి);
	}
}

శూన్య పంక్తి_macro(నిర్మాణం Token* p)
{
	ఒకవేళ(p->type & macro)
	{
		సమితి_వ్యక్తీకరణ(p->తదుపరి, p->పాఠ్యం, p->వ్యక్తీకరణ);
	}

	ఒకవేళ(ఖాళీ != p->తదుపరి)
	{
		పంక్తి_macro(p->తదుపరి);
	}
}

శూన్య hexify_string(నిర్మాణం Token* p)
{
	అక్షరం పట్టిక[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	పూర్ణాంకం i = ((strnlen(p->పాఠ్యం + 1 , max_string)/4) + 1) * 8;

	అక్షరం* d = సున్నా_జ్ఞాపకం_ఇవ్వు(max_string, పరిమాణం(అక్షరం));
	p->వ్యక్తీకరణ = d;

	అయితే(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	అయితే( i < max_string)
	{
		ఒకవేళ(0 == p->పాఠ్యం[i+1])
		{
			i = max_string;
		}
		లేకపోతే
		{
			d[2*i]  = పట్టిక[p->పాఠ్యం[i+1] / 16];
			d[2*i + 1] = పట్టిక[p->పాఠ్యం[i+1] % 16];
			i = i + 1;
		}
	}
}

శూన్య process_string(నిర్మాణం Token* p)
{
	ఒకవేళ(p->type & str)
	{
		ఒకవేళ('\'' == p->పాఠ్యం[0])
		{
			p->వ్యక్తీకరణ = p->పాఠ్యం + 1;
		}
		లేకపోతే ఒకవేళ('"' == p->పాఠ్యం[0])
		{
			hexify_string(p);
		}
	}

	ఒకవేళ(ఖాళీ != p->తదుపరి)
	{
		process_string(p->తదుపరి);
	}
}


శూన్య preserve_other(నిర్మాణం Token* p)
{
	ఒకవేళ(ఖాళీ != p->తదుపరి)
	{
		preserve_other(p->తదుపరి);
	}

	ఒకవేళ((ఖాళీ == p->వ్యక్తీకరణ) && !(p->type & macro))
	{
		p->వ్యక్తీకరణ = p->పాఠ్యం;
	}
}

uint16_t numerate_string(అక్షరం a[])
{
	అక్షరం *ptr;
	తిరిగి (uint16_t)strtol(a, &ptr, 0);
}

శూన్య eval_immediates(నిర్మాణం Token* p)
{
	ఒకవేళ(ఖాళీ != p->తదుపరి)
	{
		eval_immediates(p->తదుపరి);
	}

	ఒకవేళ((ఖాళీ == p->వ్యక్తీకరణ) && !(p->type & macro))
	{
		uint16_t విలువ;
		విలువ = numerate_string(p->పాఠ్యం);

		ఒకవేళ(('0' == p->పాఠ్యం[0]) || (0 != విలువ))
		{
			అక్షరం* c = సున్నా_జ్ఞాపకం_ఇవ్వు(5, పరిమాణం(అక్షరం));
			sprintf(c, "%04x", విలువ);
			p->వ్యక్తీకరణ = c;
		}
	}
}

శూన్య print_hex(నిర్మాణం Token* p)
{
	ఒకవేళ(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->వ్యక్తీకరణ);
	}

	ఒకవేళ(ఖాళీ != p->తదుపరి)
	{
		print_hex(p->తదుపరి);
	}
	లేకపోతే
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
	/* Make sure we have a program tape to run */
	ఒకవేళ (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		తిరిగి EXIT_FAILURE;
	}

	ఆధారం_file = fopen(argv[1], "r");

	Reached_EOF = false;
	నిర్మాణం Token* తల = ఖాళీ;
	అయితే(!Reached_EOF)
	{
		తల = Tokenize_పంక్తి(తల);
	}

	identify_macros(తల);
	పంక్తి_macro(తల);
	process_string(తల);
	eval_immediates(తల);
	preserve_other(తల);
	print_hex(తల);

	తిరిగి EXIT_SUCCESS;
}
