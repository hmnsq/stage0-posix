/* Copyright (C) 2020 Jeremiah Orians
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

#شامل <stdlib.h>
#شامل <stdio.h>
#شامل <unistd.h>
#شامل <sys/wait.h>

#تعریف_کرو FALSE 0
#تعریف_کرو TRUE 1
#تعریف_کرو max_string 4096
#تعریف_کرو max_دلائل 256

حرف** علامات;
صحیح command_مکمل;

/* Function for purging line comments */
خالی collect_comment(FILE* داخلہ)
{
	صحیح c;
	کرو
	{
		c = fgetc(داخلہ);
		اگر(EOF == c) باہر_نکلو(EXIT_FAILURE);
	} جب('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
صحیح collect_string(FILE* داخلہ, صحیح index, حرف* ہدف)
{
	صحیح c;
collect_string_loop:
	c = fgetc(داخلہ);
	اگر(EOF == c) باہر_نکلو(EXIT_FAILURE);
	ورنہ اگر('"' == c) واپس index;
	ہدف[index] = c;
	index = index + 1;
	جاؤ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
حرف* collect_token(FILE* داخلہ)
{
	حرف* token = صفر_حافظہ_دو(max_string, حجم(حرف));
	صحیح c;
	صحیح i = 0;
collect_token_loop:
	c = fgetc(داخلہ);
	اگر(EOF == c) باہر_نکلو(EXIT_SUCCESS);
	ورنہ اگر((' ' == c) || ('\t' == c)) جاؤ collect_token_مکمل;
	ورنہ اگر('\n' == c)
	{ /* Command terminates at end of line */
		command_مکمل = 1;
		جاؤ collect_token_مکمل;
	}
	ورنہ اگر('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(داخلہ, i, token);
		جاؤ collect_token_مکمل;
	}
	ورنہ اگر('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(داخلہ);
		command_مکمل = 1;
		جاؤ collect_token_مکمل;
	}
	ورنہ اگر('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(داخلہ);
		جاؤ collect_token_مکمل;
	}
	token[i] = c;
	i = i + 1;
	جاؤ collect_token_loop;

collect_token_مکمل:
	اگر(0 == i) واپس تہی;
	واپس token;
}

صحیح مین(صحیح argc, حرف** argv, حرف** envp)
{
	حرف* filename = "kaem.run";
	FILE* script = تہی;

	اگر(تہی != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	اگر(تہی == script) باہر_نکلو(EXIT_FAILURE);

main_loop:
	علامات = صفر_حافظہ_دو(max_دلائل, حجم(حرف*));

	صحیح i = 0;
	صحیح حالت_2 = 0;
	command_مکمل = 0;
	کرو
	{
		حرف* نتیجہ = collect_token(script);
		اگر(0 != نتیجہ)
		{ /* Not a comment string but an actual argument */
			علامات[i] = نتیجہ;
			i = i + 1;
		}
	} جب(0 == command_مکمل);

	اگر(0 > i) جاؤ main_loop;

	fputs(" +> ", stdout);
	صحیح j;
	برائے(j = 0; j < i; j = j + 1)
	{
		fputs(علامات[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	حرف* پروگرام = علامات[0];
	اگر(تہی == پروگرام) باہر_نکلو(EXIT_FAILURE);

	صحیح f = fork();
	اگر (f == -1) باہر_نکلو(EXIT_FAILURE);
	ورنہ اگر (f == 0)
	{ /*بچہ*/
		/* execve() returns only on error */
		execve(پروگرام, علامات, envp);
		/* Prevent infinite loops */
		باہر_نکلو(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &حالت_2, 0);

	اگر(0 == حالت_2) جاؤ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	باہر_نکلو(EXIT_FAILURE);
}
