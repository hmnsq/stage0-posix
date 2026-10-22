/* Copyright (C) 2020 Jeremiah Orians
 * Copyright (C) 2017 Jan Nieuwenhuizen <janneke@gnu.org>
 * This file is part of stage0
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
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "../M2libc/bootstrappable.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ max_string 4096

ਬਣਤਰ ਇੰਦਰਾਜ
{
	ਬਣਤਰ ਇੰਦਰਾਜ* ਅਗਲਾ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਿਸ਼ਾਨਾ;
	ਅੱਖਰ* name;
};


/* Globals */
FILE* output;
ਬਣਤਰ ਇੰਦਰਾਜ* jump_ਸਾਰਣੀ;
ਪੂਰਨ ਅਧਾਰ_ਪਤਾ;
ਪੂਰਨ ip;
ਅੱਖਰ* scratch;

ਪੂਰਨ consume_token(FILE* ਸਰੋਤ_file)
{
	ਪੂਰਨ i = 0;
	ਪੂਰਨ c = fgetc(ਸਰੋਤ_file);
	ਜਦੋਂ(!ਵਿੱਚ_ਸੈੱਟ(c, " \t\n>"))
	{
		scratch[i] = c;
		i = i + 1;
		c = fgetc(ਸਰੋਤ_file);
		require(max_string > i, "Consumed token exceeds length restriction\n");
		ਜੇ(EOF == c) ਰੋਕੋ;
	}

	ਵਾਪਸ c;
}

ਪੂਰਨ Throwaway_token(FILE* ਸਰੋਤ_file)
{
	ਪੂਰਨ c;
	ਕਰੋ
	{
		c = fgetc(ਸਰੋਤ_file);
		ਜੇ(EOF == c) ਰੋਕੋ;
	} ਜਦੋਂ(!ਵਿੱਚ_ਸੈੱਟ(c, " \t\n>"));

	ਵਾਪਸ c;
}

ਪੂਰਨ ਲੰਮਾਈ(ਅੱਖਰ* s)
{
	ਪੂਰਨ i = 0;
	ਜਦੋਂ(0 != s[i]) i = i + 1;
	ਵਾਪਸ i;
}

ਖਾਲੀ Clear_Scratch(ਅੱਖਰ* s)
{
	ਕਰੋ
	{
		s[0] = 0;
		s = s + 1;
	} ਜਦੋਂ(0 != s[0]);
}

ਖਾਲੀ Copy_String(ਅੱਖਰ* a, ਅੱਖਰ* b)
{
	ਜਦੋਂ(0 != a[0])
	{
		b[0] = a[0];
		a = a + 1;
		b = b + 1;
	}
}

ਬਿਨਾਨਿਸ਼ਾਨੀ Get_ਨਿਸ਼ਾਨਾ(ਅੱਖਰ* c)
{
	ਬਣਤਰ ਇੰਦਰਾਜ* i;
	ਲਈ(i = jump_ਸਾਰਣੀ; ਨਿਰਾ != i; i = i->ਅਗਲਾ)
	{
		ਜੇ(ਮੇਲ(c, i->name))
		{
			ਵਾਪਸ i->ਨਿਸ਼ਾਨਾ;
		}
	}
	fputs("Target label ", stderr);
	fputs(c, stderr);
	fputs(" is not valid\n", stderr);
	ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
}

ਪੂਰਨ storeLabel(FILE* ਸਰੋਤ_file, ਪੂਰਨ ip)
{
	ਬਣਤਰ ਇੰਦਰਾਜ* ਇੰਦਰਾਜ = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(1, ਆਕਾਰ(ਬਣਤਰ ਇੰਦਰਾਜ));

	/* Ensure we have target address */
	ਇੰਦਰਾਜ->ਨਿਸ਼ਾਨਾ = ip;

	/* Prepend to list */
	ਇੰਦਰਾਜ->ਅਗਲਾ = jump_ਸਾਰਣੀ;
	jump_ਸਾਰਣੀ = ਇੰਦਰਾਜ;

	/* Store string */
	ਪੂਰਨ c = consume_token(ਸਰੋਤ_file);
	ਇੰਦਰਾਜ->name = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਲੰਮਾਈ(scratch) + 1, ਆਕਾਰ(ਅੱਖਰ));
	Copy_String(scratch, ਇੰਦਰਾਜ->name);
	Clear_Scratch(scratch);

	ਵਾਪਸ c;
}

ਖਾਲੀ output_ਸੂਚਕ(ਪੂਰਨ displacement, ਪੂਰਨ ਸੰਖਿਆ_ਦਾ_ਬਾਈਟਾਂ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ = displacement;

	ਜਦੋਂ(ਸੰਖਿਆ_ਦਾ_ਬਾਈਟਾਂ > 0)
	{
		ਬਿਨਾਨਿਸ਼ਾਨੀ byte = ਮੁੱਲ % 256;
		ਮੁੱਲ = ਮੁੱਲ / 256;
		fputc(byte, output);
		ਸੰਖਿਆ_ਦਾ_ਬਾਈਟਾਂ = ਸੰਖਿਆ_ਦਾ_ਬਾਈਟਾਂ - 1;
	}
}

ਖਾਲੀ Update_ਸੂਚਕ(ਅੱਖਰ ch)
{
	/* Calculate pointer size*/
	ਜੇ(ਵਿੱਚ_ਸੈੱਟ(ch, "%&")) ip = ip + 4; /* Deal with % and & */
	ਨਹੀਂਤਾਂ ਜੇ(ਵਿੱਚ_ਸੈੱਟ(ch, "@$")) ip = ip + 2; /* Deal with @ and $ */
	ਨਹੀਂਤਾਂ ਜੇ('!' == ch) ip = ip + 1; /* Deal with ! */
	ਨਹੀਂਤਾਂ ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
}

ਖਾਲੀ store_ਸੂਚਕ(ਅੱਖਰ ch, FILE* ਸਰੋਤ_file)
{
	/* Get string of pointer */
	Clear_Scratch(scratch);
	Update_ਸੂਚਕ(ch);
	ਪੂਰਨ ਅਧਾਰ_sep_p = consume_token(ਸਰੋਤ_file);

	/* Lookup token */
	ਪੂਰਨ ਨਿਸ਼ਾਨਾ = Get_ਨਿਸ਼ਾਨਾ(scratch);
	ਪੂਰਨ displacement;

	ਪੂਰਨ ਅਧਾਰ = ip;

	/* Change relative base address to :<base> */
	ਜੇ ('>' == ਅਧਾਰ_sep_p)
	{
		Clear_Scratch(scratch);
		consume_token (ਸਰੋਤ_file);
		ਅਧਾਰ = Get_ਨਿਸ਼ਾਨਾ (scratch);

		/* Force universality of behavior */
		displacement = (ਨਿਸ਼ਾਨਾ - ਅਧਾਰ);
	}
	ਨਹੀਂਤਾਂ
	{
		displacement = ਅਧਾਰ - ਨਿਸ਼ਾਨਾ;
	}

	/* output calculated difference */
	ਜੇ('!' == ch) output_ਸੂਚਕ(displacement, 1); /* Deal with ! */
	ਨਹੀਂਤਾਂ ਜੇ('$' == ch) output_ਸੂਚਕ(ਨਿਸ਼ਾਨਾ, 2); /* Deal with $ */
	ਨਹੀਂਤਾਂ ਜੇ('@' == ch) output_ਸੂਚਕ(displacement, 2); /* Deal with @ */
	ਨਹੀਂਤਾਂ ਜੇ('&' == ch) output_ਸੂਚਕ(ਨਿਸ਼ਾਨਾ, 4); /* Deal with & */
	ਨਹੀਂਤਾਂ ਜੇ('%' == ch) output_ਸੂਚਕ(displacement, 4);  /* Deal with % */
	ਨਹੀਂਤਾਂ ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
}

ਖਾਲੀ ਸਤਰ_Comment(FILE* ਸਰੋਤ_file)
{
	ਪੂਰਨ c = fgetc(ਸਰੋਤ_file);
	ਜਦੋਂ(!ਵਿੱਚ_ਸੈੱਟ(c, "\n\r"))
	{
		ਜੇ(EOF == c) ਰੋਕੋ;
		c = fgetc(ਸਰੋਤ_file);
	}
}

ਪੂਰਨ hex(ਪੂਰਨ c, FILE* ਸਰੋਤ_file)
{
	ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "0123456789")) ਵਾਪਸ (c - 48);
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "abcdef")) ਵਾਪਸ (c - 87);
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "ABCDEF")) ਵਾਪਸ (c - 55);
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, "#;")) ਸਤਰ_Comment(ਸਰੋਤ_file);
	ਵਾਪਸ -1;
}

ਪੂਰਨ hold;
ਪੂਰਨ toggle;
ਖਾਲੀ process_byte(ਅੱਖਰ c, FILE* ਸਰੋਤ_file, ਪੂਰਨ ਲਿਖ)
{
	ਜੇ(0 <= hex(c, ਸਰੋਤ_file))
	{
		ਜੇ(toggle)
		{
			ਜੇ(ਲਿਖ) fputc(((hold * 16)) + hex(c, ਸਰੋਤ_file), output);
			ip = ip + 1;
			hold = 0;
		}
		ਨਹੀਂਤਾਂ
		{
			hold = hex(c, ਸਰੋਤ_file);
		}
		toggle = !toggle;
	}
}

ਖਾਲੀ ਪਹਿਲਾ_pass(FILE* ਨਿਵੇਸ਼)
{
	toggle = FALSE;
	ਪੂਰਨ c;
	ਲਈ(c = fgetc(ਨਿਵੇਸ਼); EOF != c; c = fgetc(ਨਿਵੇਸ਼))
	{
		/* Check for and deal with label */
		ਜੇ(':' == c)
		{
			c = storeLabel(ਨਿਵੇਸ਼, ip);
		}

		/* check for and deal with relative/absolute pointers to labels */
		ਜੇ(ਵਿੱਚ_ਸੈੱਟ(c, "!@$~%&"))
		{ /* deal with 1byte pointer !; 2byte pointers (@ and $); 3byte pointers ~; 4byte pointers (% and &) */
			Update_ਸੂਚਕ(c);
			c = Throwaway_token(ਨਿਵੇਸ਼);
			ਜੇ ('>' == c)
			{ /* deal with label>base */
				c = Throwaway_token(ਨਿਵੇਸ਼);
			}
		}
		ਨਹੀਂਤਾਂ process_byte(c, ਨਿਵੇਸ਼, FALSE);
	}
}

ਖਾਲੀ ਦੂਜਾ_pass(FILE* ਨਿਵੇਸ਼)
{
	toggle = FALSE;
	hold = 0;

	ਪੂਰਨ c;
	ਲਈ(c = fgetc(ਨਿਵੇਸ਼); EOF != c; c = fgetc(ਨਿਵੇਸ਼))
	{
		ਜੇ(':' == c) c = Throwaway_token(ਨਿਵੇਸ਼); /* Deal with : */
		ਨਹੀਂਤਾਂ ਜੇ(ਵਿੱਚ_ਸੈੱਟ(c, "!@$~%&")) store_ਸੂਚਕ(c, ਨਿਵੇਸ਼);  /* Deal with !, @, $, ~, % and & */
		ਨਹੀਂਤਾਂ process_byte(c, ਨਿਵੇਸ਼, TRUE);
	}
}

/* Standard C main program */
ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
	jump_ਸਾਰਣੀ = ਨਿਰਾ;
	ਅਧਾਰ_ਪਤਾ = 0x8048000;
	output = stdout;
	scratch = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(max_string + 1, ਆਕਾਰ(ਅੱਖਰ));

	FILE* ਨਿਵੇਸ਼ = fopen(argv[1], "r");
	output = stdout;
	ਜੇ(ਨਿਰਾ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = ਅਧਾਰ_ਪਤਾ;
	ਪਹਿਲਾ_pass(ਨਿਵੇਸ਼);
	rewind(ਨਿਵੇਸ਼);

	/* Fix all the references*/
	ip = ਅਧਾਰ_ਪਤਾ;
	ਦੂਜਾ_pass(ਨਿਵੇਸ਼);

	/* Set file as executable */
	ਵਾਪਸ EXIT_SUCCESS;
}
