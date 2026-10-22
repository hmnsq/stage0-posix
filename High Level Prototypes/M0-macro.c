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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ max_string 63

FILE* ਸਰੋਤ_file;
bool Reached_EOF;

ਬਣਤਰ Token
{
	ਬਣਤਰ Token* ਅਗਲਾ;
	uint8_t type;
	ਅੱਖਰ* ਪਾਠ;
	ਅੱਖਰ* ਵਿਅੰਜਕ;
};

ਗਿਣਤੀ type
{
	macro = 1,
	str = (1 << 1)
};

ਬਣਤਰ Token* newToken()
{
	ਬਣਤਰ Token* p;

	p = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ (1, ਆਕਾਰ (ਬਣਤਰ Token));
	ਜੇ (ਨਿਰਾ == p)
	{
		fprintf (stderr, "calloc failed.\n");
		ਬਾਹਰ_ਨਿਕਲੋ (EXIT_FAILURE);
	}

	ਵਾਪਸ p;
}

ਬਣਤਰ Token* ਜੋੜੋ_Token(ਬਣਤਰ Token* ਸਿਰ, ਬਣਤਰ Token* p)
{
	ਜੇ(ਨਿਰਾ == ਸਿਰ)
	{
		ਵਾਪਸ p;
	}
	ਜੇ(ਨਿਰਾ == ਸਿਰ->ਅਗਲਾ)
	{
		ਸਿਰ->ਅਗਲਾ = p;
	}
	ਨਹੀਂਤਾਂ
	{
		ਜੋੜੋ_Token(ਸਿਰ->ਅਗਲਾ, p);
	}
	ਵਾਪਸ ਸਿਰ;
}

ਖਾਲੀ purge_ਸਤਰ_Comment()
{
	ਪੂਰਨ c = fgetc(ਸਰੋਤ_file);
	ਜਦੋਂ((10 != c) && (13 != c))
	{
		c = fgetc(ਸਰੋਤ_file);
	}
}

ਅੱਖਰ* store_atom(ਅੱਖਰ c)
{
	ਅੱਖਰ* store = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(max_string + 1, ਆਕਾਰ(ਅੱਖਰ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ਕਰੋ
	{
		store[i] = (ਅੱਖਰ)ch;
		ch = fgetc(ਸਰੋਤ_file);
		i = i + 1;
	} ਜਦੋਂ ((9 != ch) && (10 != ch) && (32 != ch));

	ਵਾਪਸ store;
}

ਅੱਖਰ* store_string(ਅੱਖਰ c)
{
	ਅੱਖਰ* store = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(max_string + 1, ਆਕਾਰ(ਅੱਖਰ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ਕਰੋ
	{
		store[i] = (ਅੱਖਰ)ch;
		i = i + 1;
		ch = fgetc(ਸਰੋਤ_file);
	} ਜਦੋਂ(ch != c);

	ਵਾਪਸ store;
}

ਬਣਤਰ Token* Tokenize_ਸਤਰ(ਬਣਤਰ Token* ਸਿਰ)
{

	int32_t c;
	c = fgetc(ਸਰੋਤ_file);

	ਜੇ((35 == c) || (59 == c))
	{
		purge_ਸਤਰ_Comment();
		ਵਾਪਸ Tokenize_ਸਤਰ(ਸਿਰ);
	}

	ਜੇ((9 == c) || (10 == c) || (32 == c))
	{
		ਵਾਪਸ Tokenize_ਸਤਰ(ਸਿਰ);
	}

	ਬਣਤਰ Token* p = newToken();
	ਜੇ(-1 == c)
	{
		Reached_EOF = true;
		ਮੁਕਤ(p);
		ਵਾਪਸ ਸਿਰ;
	}
	ਨਹੀਂਤਾਂ ਜੇ((34 == c) || (39 == c))
	{
		p->ਪਾਠ = store_string(c);
		p->type = str;
	}
	ਨਹੀਂਤਾਂ
	{
		p->ਪਾਠ = store_atom(c);
	}

	ਵਾਪਸ ਜੋੜੋ_Token(ਸਿਰ, p);
}

ਖਾਲੀ ਸੈੱਟ_ਵਿਅੰਜਕ(ਬਣਤਰ Token* p, ਅੱਖਰ ਮੇਲ[], ਅੱਖਰ Exp[])
{
	/* Leave macros alone */
	ਜੇ((p->type & macro))
	{
		ਸੈੱਟ_ਵਿਅੰਜਕ(p->ਅਗਲਾ, ਮੇਲ, Exp);
		ਵਾਪਸ;
	}

	/* Only if there is an exact match replace */
	ਜੇ(0 == ਲੜੀ_ਤੁਲਨਾ(p->ਪਾਠ, ਮੇਲ, max_string))
	{
		p->ਵਿਅੰਜਕ = Exp;
	}

	ਜੇ(ਨਿਰਾ != p->ਅਗਲਾ)
	{
		ਸੈੱਟ_ਵਿਅੰਜਕ(p->ਅਗਲਾ, ਮੇਲ, Exp);
	}

}

ਖਾਲੀ identify_macros(ਬਣਤਰ Token* p)
{
	ਜੇ(0 == ਲੜੀ_ਤੁਲਨਾ(p->ਪਾਠ, "DEFINE", max_string))
	{
		p->type = macro;
		p->ਪਾਠ = p->ਅਗਲਾ->ਪਾਠ;
		ਜੇ(p->ਅਗਲਾ->ਅਗਲਾ->type & str)
		{
			p->ਵਿਅੰਜਕ = p->ਅਗਲਾ->ਅਗਲਾ->ਪਾਠ + 1;
		}
		ਨਹੀਂਤਾਂ
		{
			p->ਵਿਅੰਜਕ = p->ਅਗਲਾ->ਅਗਲਾ->ਪਾਠ;
		}
		p->ਅਗਲਾ = p->ਅਗਲਾ->ਅਗਲਾ->ਅਗਲਾ;
	}

	ਜੇ(ਨਿਰਾ != p->ਅਗਲਾ)
	{
		identify_macros(p->ਅਗਲਾ);
	}
}

ਖਾਲੀ ਸਤਰ_macro(ਬਣਤਰ Token* p)
{
	ਜੇ(p->type & macro)
	{
		ਸੈੱਟ_ਵਿਅੰਜਕ(p->ਅਗਲਾ, p->ਪਾਠ, p->ਵਿਅੰਜਕ);
	}

	ਜੇ(ਨਿਰਾ != p->ਅਗਲਾ)
	{
		ਸਤਰ_macro(p->ਅਗਲਾ);
	}
}

ਖਾਲੀ hexify_string(ਬਣਤਰ Token* p)
{
	ਅੱਖਰ ਸਾਰਣੀ[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	ਪੂਰਨ i = ((strnlen(p->ਪਾਠ + 1 , max_string)/4) + 1) * 8;

	ਅੱਖਰ* d = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(max_string, ਆਕਾਰ(ਅੱਖਰ));
	p->ਵਿਅੰਜਕ = d;

	ਜਦੋਂ(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	ਜਦੋਂ( i < max_string)
	{
		ਜੇ(0 == p->ਪਾਠ[i+1])
		{
			i = max_string;
		}
		ਨਹੀਂਤਾਂ
		{
			d[2*i]  = ਸਾਰਣੀ[p->ਪਾਠ[i+1] / 16];
			d[2*i + 1] = ਸਾਰਣੀ[p->ਪਾਠ[i+1] % 16];
			i = i + 1;
		}
	}
}

ਖਾਲੀ process_string(ਬਣਤਰ Token* p)
{
	ਜੇ(p->type & str)
	{
		ਜੇ('\'' == p->ਪਾਠ[0])
		{
			p->ਵਿਅੰਜਕ = p->ਪਾਠ + 1;
		}
		ਨਹੀਂਤਾਂ ਜੇ('"' == p->ਪਾਠ[0])
		{
			hexify_string(p);
		}
	}

	ਜੇ(ਨਿਰਾ != p->ਅਗਲਾ)
	{
		process_string(p->ਅਗਲਾ);
	}
}


ਖਾਲੀ preserve_other(ਬਣਤਰ Token* p)
{
	ਜੇ(ਨਿਰਾ != p->ਅਗਲਾ)
	{
		preserve_other(p->ਅਗਲਾ);
	}

	ਜੇ((ਨਿਰਾ == p->ਵਿਅੰਜਕ) && !(p->type & macro))
	{
		p->ਵਿਅੰਜਕ = p->ਪਾਠ;
	}
}

uint16_t numerate_string(ਅੱਖਰ a[])
{
	ਅੱਖਰ *ptr;
	ਵਾਪਸ (uint16_t)strtol(a, &ptr, 0);
}

ਖਾਲੀ eval_immediates(ਬਣਤਰ Token* p)
{
	ਜੇ(ਨਿਰਾ != p->ਅਗਲਾ)
	{
		eval_immediates(p->ਅਗਲਾ);
	}

	ਜੇ((ਨਿਰਾ == p->ਵਿਅੰਜਕ) && !(p->type & macro))
	{
		uint16_t ਮੁੱਲ;
		ਮੁੱਲ = numerate_string(p->ਪਾਠ);

		ਜੇ(('0' == p->ਪਾਠ[0]) || (0 != ਮੁੱਲ))
		{
			ਅੱਖਰ* c = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(5, ਆਕਾਰ(ਅੱਖਰ));
			sprintf(c, "%04x", ਮੁੱਲ);
			p->ਵਿਅੰਜਕ = c;
		}
	}
}

ਖਾਲੀ print_hex(ਬਣਤਰ Token* p)
{
	ਜੇ(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->ਵਿਅੰਜਕ);
	}

	ਜੇ(ਨਿਰਾ != p->ਅਗਲਾ)
	{
		print_hex(p->ਅਗਲਾ);
	}
	ਨਹੀਂਤਾਂ
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
	/* Make sure we have a program tape to run */
	ਜੇ (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		ਵਾਪਸ EXIT_FAILURE;
	}

	ਸਰੋਤ_file = fopen(argv[1], "r");

	Reached_EOF = false;
	ਬਣਤਰ Token* ਸਿਰ = ਨਿਰਾ;
	ਜਦੋਂ(!Reached_EOF)
	{
		ਸਿਰ = Tokenize_ਸਤਰ(ਸਿਰ);
	}

	identify_macros(ਸਿਰ);
	ਸਤਰ_macro(ਸਿਰ);
	process_string(ਸਿਰ);
	eval_immediates(ਸਿਰ);
	preserve_other(ਸਿਰ);
	print_hex(ਸਿਰ);

	ਵਾਪਸ EXIT_SUCCESS;
}
