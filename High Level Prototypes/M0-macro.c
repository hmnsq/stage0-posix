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
#ವ್ಯಾಖ್ಯಾನಿಸು max_string 63

FILE* ಮೂಲ_file;
bool Reached_EOF;

ರಚನೆ Token
{
	ರಚನೆ Token* ಮುಂದಿನ;
	uint8_t type;
	ಅಕ್ಷರ* ಪಠ್ಯ;
	ಅಕ್ಷರ* ಅಭಿವ್ಯಕ್ತಿ;
};

ಎಣಿಕೆ type
{
	macro = 1,
	str = (1 << 1)
};

ರಚನೆ Token* newToken()
{
	ರಚನೆ Token* p;

	p = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು (1, ಗಾತ್ರ (ರಚನೆ Token));
	ಆದರೆ (ಖಾಲಿ == p)
	{
		fprintf (stderr, "calloc failed.\n");
		ನಿರ್ಗಮಿಸು (EXIT_FAILURE);
	}

	ಹಿಂತಿರುಗಿ p;
}

ರಚನೆ Token* ಕೂಡಿಸು_Token(ರಚನೆ Token* ತಲೆ, ರಚನೆ Token* p)
{
	ಆದರೆ(ಖಾಲಿ == ತಲೆ)
	{
		ಹಿಂತಿರುಗಿ p;
	}
	ಆದರೆ(ಖಾಲಿ == ತಲೆ->ಮುಂದಿನ)
	{
		ತಲೆ->ಮುಂದಿನ = p;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
		ಕೂಡಿಸು_Token(ತಲೆ->ಮುಂದಿನ, p);
	}
	ಹಿಂತಿರುಗಿ ತಲೆ;
}

ಶೂನ್ಯ purge_ಸಾಲು_Comment()
{
	ಪೂರ್ಣಾಂಕ c = fgetc(ಮೂಲ_file);
	ಆಗ((10 != c) && (13 != c))
	{
		c = fgetc(ಮೂಲ_file);
	}
}

ಅಕ್ಷರ* store_atom(ಅಕ್ಷರ c)
{
	ಅಕ್ಷರ* store = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(max_string + 1, ಗಾತ್ರ(ಅಕ್ಷರ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ಮಾಡು
	{
		store[i] = (ಅಕ್ಷರ)ch;
		ch = fgetc(ಮೂಲ_file);
		i = i + 1;
	} ಆಗ ((9 != ch) && (10 != ch) && (32 != ch));

	ಹಿಂತಿರುಗಿ store;
}

ಅಕ್ಷರ* store_string(ಅಕ್ಷರ c)
{
	ಅಕ್ಷರ* store = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(max_string + 1, ಗಾತ್ರ(ಅಕ್ಷರ));
	int32_t ch;
	uint32_t i = 0;
	ch = c;
	ಮಾಡು
	{
		store[i] = (ಅಕ್ಷರ)ch;
		i = i + 1;
		ch = fgetc(ಮೂಲ_file);
	} ಆಗ(ch != c);

	ಹಿಂತಿರುಗಿ store;
}

ರಚನೆ Token* Tokenize_ಸಾಲು(ರಚನೆ Token* ತಲೆ)
{

	int32_t c;
	c = fgetc(ಮೂಲ_file);

	ಆದರೆ((35 == c) || (59 == c))
	{
		purge_ಸಾಲು_Comment();
		ಹಿಂತಿರುಗಿ Tokenize_ಸಾಲು(ತಲೆ);
	}

	ಆದರೆ((9 == c) || (10 == c) || (32 == c))
	{
		ಹಿಂತಿರುಗಿ Tokenize_ಸಾಲು(ತಲೆ);
	}

	ರಚನೆ Token* p = newToken();
	ಆದರೆ(-1 == c)
	{
		Reached_EOF = true;
		ಬಿಡುಗಡೆ(p);
		ಹಿಂತಿರುಗಿ ತಲೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((34 == c) || (39 == c))
	{
		p->ಪಠ್ಯ = store_string(c);
		p->type = str;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
		p->ಪಠ್ಯ = store_atom(c);
	}

	ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_Token(ತಲೆ, p);
}

ಶೂನ್ಯ ಗಣ_ಅಭಿವ್ಯಕ್ತಿ(ರಚನೆ Token* p, ಅಕ್ಷರ ಹೊಂದಿಕೆ[], ಅಕ್ಷರ Exp[])
{
	/* Leave macros alone */
	ಆದರೆ((p->type & macro))
	{
		ಗಣ_ಅಭಿವ್ಯಕ್ತಿ(p->ಮುಂದಿನ, ಹೊಂದಿಕೆ, Exp);
		ಹಿಂತಿರುಗಿ;
	}

	/* Only if there is an exact match replace */
	ಆದರೆ(0 == ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p->ಪಠ್ಯ, ಹೊಂದಿಕೆ, max_string))
	{
		p->ಅಭಿವ್ಯಕ್ತಿ = Exp;
	}

	ಆದರೆ(ಖಾಲಿ != p->ಮುಂದಿನ)
	{
		ಗಣ_ಅಭಿವ್ಯಕ್ತಿ(p->ಮುಂದಿನ, ಹೊಂದಿಕೆ, Exp);
	}

}

ಶೂನ್ಯ identify_macros(ರಚನೆ Token* p)
{
	ಆದರೆ(0 == ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p->ಪಠ್ಯ, "DEFINE", max_string))
	{
		p->type = macro;
		p->ಪಠ್ಯ = p->ಮುಂದಿನ->ಪಠ್ಯ;
		ಆದರೆ(p->ಮುಂದಿನ->ಮುಂದಿನ->type & str)
		{
			p->ಅಭಿವ್ಯಕ್ತಿ = p->ಮುಂದಿನ->ಮುಂದಿನ->ಪಠ್ಯ + 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			p->ಅಭಿವ್ಯಕ್ತಿ = p->ಮುಂದಿನ->ಮುಂದಿನ->ಪಠ್ಯ;
		}
		p->ಮುಂದಿನ = p->ಮುಂದಿನ->ಮುಂದಿನ->ಮುಂದಿನ;
	}

	ಆದರೆ(ಖಾಲಿ != p->ಮುಂದಿನ)
	{
		identify_macros(p->ಮುಂದಿನ);
	}
}

ಶೂನ್ಯ ಸಾಲು_macro(ರಚನೆ Token* p)
{
	ಆದರೆ(p->type & macro)
	{
		ಗಣ_ಅಭಿವ್ಯಕ್ತಿ(p->ಮುಂದಿನ, p->ಪಠ್ಯ, p->ಅಭಿವ್ಯಕ್ತಿ);
	}

	ಆದರೆ(ಖಾಲಿ != p->ಮುಂದಿನ)
	{
		ಸಾಲು_macro(p->ಮುಂದಿನ);
	}
}

ಶೂನ್ಯ hexify_string(ರಚನೆ Token* p)
{
	ಅಕ್ಷರ ಕೋಷ್ಟಕ[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
	ಪೂರ್ಣಾಂಕ i = ((strnlen(p->ಪಠ್ಯ + 1 , max_string)/4) + 1) * 8;

	ಅಕ್ಷರ* d = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(max_string, ಗಾತ್ರ(ಅಕ್ಷರ));
	p->ಅಭಿವ್ಯಕ್ತಿ = d;

	ಆಗ(0 < i)
	{
		i = i - 1;
		d[i] = 0x30;
	}

	ಆಗ( i < max_string)
	{
		ಆದರೆ(0 == p->ಪಠ್ಯ[i+1])
		{
			i = max_string;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			d[2*i]  = ಕೋಷ್ಟಕ[p->ಪಠ್ಯ[i+1] / 16];
			d[2*i + 1] = ಕೋಷ್ಟಕ[p->ಪಠ್ಯ[i+1] % 16];
			i = i + 1;
		}
	}
}

ಶೂನ್ಯ process_string(ರಚನೆ Token* p)
{
	ಆದರೆ(p->type & str)
	{
		ಆದರೆ('\'' == p->ಪಠ್ಯ[0])
		{
			p->ಅಭಿವ್ಯಕ್ತಿ = p->ಪಠ್ಯ + 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ('"' == p->ಪಠ್ಯ[0])
		{
			hexify_string(p);
		}
	}

	ಆದರೆ(ಖಾಲಿ != p->ಮುಂದಿನ)
	{
		process_string(p->ಮುಂದಿನ);
	}
}


ಶೂನ್ಯ preserve_other(ರಚನೆ Token* p)
{
	ಆದರೆ(ಖಾಲಿ != p->ಮುಂದಿನ)
	{
		preserve_other(p->ಮುಂದಿನ);
	}

	ಆದರೆ((ಖಾಲಿ == p->ಅಭಿವ್ಯಕ್ತಿ) && !(p->type & macro))
	{
		p->ಅಭಿವ್ಯಕ್ತಿ = p->ಪಠ್ಯ;
	}
}

uint16_t numerate_string(ಅಕ್ಷರ a[])
{
	ಅಕ್ಷರ *ptr;
	ಹಿಂತಿರುಗಿ (uint16_t)strtol(a, &ptr, 0);
}

ಶೂನ್ಯ eval_immediates(ರಚನೆ Token* p)
{
	ಆದರೆ(ಖಾಲಿ != p->ಮುಂದಿನ)
	{
		eval_immediates(p->ಮುಂದಿನ);
	}

	ಆದರೆ((ಖಾಲಿ == p->ಅಭಿವ್ಯಕ್ತಿ) && !(p->type & macro))
	{
		uint16_t ಮೌಲ್ಯ;
		ಮೌಲ್ಯ = numerate_string(p->ಪಠ್ಯ);

		ಆದರೆ(('0' == p->ಪಠ್ಯ[0]) || (0 != ಮೌಲ್ಯ))
		{
			ಅಕ್ಷರ* c = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(5, ಗಾತ್ರ(ಅಕ್ಷರ));
			sprintf(c, "%04x", ಮೌಲ್ಯ);
			p->ಅಭಿವ್ಯಕ್ತಿ = c;
		}
	}
}

ಶೂನ್ಯ print_hex(ರಚನೆ Token* p)
{
	ಆದರೆ(p->type ^ macro)
	{
		fprintf(stdout, "\n%s", p->ಅಭಿವ್ಯಕ್ತಿ);
	}

	ಆದರೆ(ಖಾಲಿ != p->ಮುಂದಿನ)
	{
		print_hex(p->ಮುಂದಿನ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
		fprintf(stdout, "\n");
	}
}

/* Standard C main program */
ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
	/* Make sure we have a program tape to run */
	ಆದರೆ (argc < 2)
	{
		fprintf(stderr, "Usage: %s $FileName\nWhere $FileName is the name of the paper tape of the program being run\n", argv[0]);
		ಹಿಂತಿರುಗಿ EXIT_FAILURE;
	}

	ಮೂಲ_file = fopen(argv[1], "r");

	Reached_EOF = false;
	ರಚನೆ Token* ತಲೆ = ಖಾಲಿ;
	ಆಗ(!Reached_EOF)
	{
		ತಲೆ = Tokenize_ಸಾಲು(ತಲೆ);
	}

	identify_macros(ತಲೆ);
	ಸಾಲು_macro(ತಲೆ);
	process_string(ತಲೆ);
	eval_immediates(ತಲೆ);
	preserve_other(ತಲೆ);
	print_hex(ತಲೆ);

	ಹಿಂತಿರುಗಿ EXIT_SUCCESS;
}
