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
	ਅੱਖਰ name;
};


/* Globals */
FILE* output;
ਬਣਤਰ ਇੰਦਰਾਜ* jump_ਸਾਰਣੀ;
ਪੂਰਨ ip;

ਬਿਨਾਨਿਸ਼ਾਨੀ Get_ਨਿਸ਼ਾਨਾ(ਅੱਖਰ c)
{
	ਬਣਤਰ ਇੰਦਰਾਜ* i;
	ਲਈ(i = jump_ਸਾਰਣੀ; ਨਿਰਾ != i; i = i->ਅਗਲਾ)
	{
		ਜੇ(c == i->name)
		{
			ਵਾਪਸ i->ਨਿਸ਼ਾਨਾ;
		}
	}
	ਵਾਪਸ 0;
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
	ਇੰਦਰਾਜ->name = fgetc(ਸਰੋਤ_file);
	ਵਾਪਸ -1;
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
	ਜੇ(ਵਿੱਚ_ਸੈੱਟ(ch, "%")) ip = ip + 4;
	ਨਹੀਂਤਾਂ ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
}

ਖਾਲੀ store_ਸੂਚਕ(ਅੱਖਰ ch)
{
	/* Get string of pointer */
	Update_ਸੂਚਕ(ch);

	/* Lookup token */
	ਪੂਰਨ ਨਿਸ਼ਾਨਾ = Get_ਨਿਸ਼ਾਨਾ(ch);
	ਪੂਰਨ displacement;

	/* Change relative base address to :<base> */
	displacement = (ਨਿਸ਼ਾਨਾ - ip);

	/* output calculated difference */
	ਜੇ('%' == ch) output_ਸੂਚਕ(displacement, 4);  /* Deal with % */
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
		ਜੇ(ਵਿੱਚ_ਸੈੱਟ(c, "%"))
		{ /* deal 4byte pointer (%) */
			Update_ਸੂਚਕ(c);
			c = fgetc(ਨਿਵੇਸ਼);
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
		ਜੇ(':' == c) c = fgetc(ਨਿਵੇਸ਼); /* Deal with : */
		ਨਹੀਂਤਾਂ ਜੇ(ਵਿੱਚ_ਸੈੱਟ(c, "%")) store_ਸੂਚਕ(c);  /* Deal with %*/
		ਨਹੀਂਤਾਂ process_byte(c, ਨਿਵੇਸ਼, TRUE);
	}
}

/* Standard C main program */
ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
	jump_ਸਾਰਣੀ = ਨਿਰਾ;
	output = stdout;

	FILE* ਨਿਵੇਸ਼ = fopen(argv[1], "r");
	output = stdout;
	ਜੇ(ਨਿਰਾ != argv[2])
	{
		output = fopen(argv[2], "w");
		chmod(argv[2], 0700);
	}

	/* Get all of the labels */
	ip = 0;
	ਪਹਿਲਾ_pass(ਨਿਵੇਸ਼);
	rewind(ਨਿਵੇਸ਼);

	/* Fix all the references*/
	ip = 0;
	ਦੂਜਾ_pass(ਨਿਵੇਸ਼);

	/* Set file as executable */
	ਵਾਪਸ EXIT_SUCCESS;
}
