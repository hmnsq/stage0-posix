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

#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن <unistd.h>
#ضمن <sys/wait.h>

#تعریف_کن FALSE 0
#تعریف_کن TRUE 1
#تعریف_کن max_string 4096
#تعریف_کن max_آرگومانها 256

حرف** نشانهها;
صحيح command_انجامشده;

/* Function for purging line comments */
فراغ collect_comment(FILE* درون_داد)
{
	صحيح c;
	افعل
	{
		c = fgetc(درون_داد);
		إذا(EOF == c) خروج(EXIT_FAILURE);
	} طالما('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
صحيح collect_string(FILE* درون_داد, صحيح index, حرف* هدف)
{
	صحيح c;
collect_string_loop:
	c = fgetc(درون_داد);
	إذا(EOF == c) خروج(EXIT_FAILURE);
	وإلا إذا('"' == c) أرجع index;
	هدف[index] = c;
	index = index + 1;
	اذهب collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
حرف* collect_token(FILE* درون_داد)
{
	حرف* token = حافظه_صفر_بده(max_string, حجم(حرف));
	صحيح c;
	صحيح i = 0;
collect_token_loop:
	c = fgetc(درون_داد);
	إذا(EOF == c) خروج(EXIT_SUCCESS);
	وإلا إذا((' ' == c) || ('\t' == c)) اذهب collect_token_انجامشده;
	وإلا إذا('\n' == c)
	{ /* Command terminates at end of line */
		command_انجامشده = 1;
		اذهب collect_token_انجامشده;
	}
	وإلا إذا('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(درون_داد, i, token);
		اذهب collect_token_انجامشده;
	}
	وإلا إذا('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(درون_داد);
		command_انجامشده = 1;
		اذهب collect_token_انجامشده;
	}
	وإلا إذا('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(درون_داد);
		اذهب collect_token_انجامشده;
	}
	token[i] = c;
	i = i + 1;
	اذهب collect_token_loop;

collect_token_انجامشده:
	إذا(0 == i) أرجع عدم;
	أرجع token;
}

صحيح اصلی(صحيح argc, حرف** argv, حرف** envp)
{
	حرف* filename = "kaem.run";
	FILE* script = عدم;

	إذا(عدم != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	إذا(عدم == script) خروج(EXIT_FAILURE);

main_loop:
	نشانهها = حافظه_صفر_بده(max_آرگومانها, حجم(حرف*));

	صحيح i = 0;
	صحيح وضعیت = 0;
	command_انجامشده = 0;
	افعل
	{
		حرف* نتیجه = collect_token(script);
		إذا(0 != نتیجه)
		{ /* Not a comment string but an actual argument */
			نشانهها[i] = نتیجه;
			i = i + 1;
		}
	} طالما(0 == command_انجامشده);

	إذا(0 > i) اذهب main_loop;

	fputs(" +> ", stdout);
	صحيح j;
	لكل(j = 0; j < i; j = j + 1)
	{
		fputs(نشانهها[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	حرف* برنامه = نشانهها[0];
	إذا(عدم == برنامه) خروج(EXIT_FAILURE);

	صحيح f = fork();
	إذا (f == -1) خروج(EXIT_FAILURE);
	وإلا إذا (f == 0)
	{ /*فرزند*/
		/* execve() returns only on error */
		execve(برنامه, نشانهها, envp);
		/* Prevent infinite loops */
		خروج(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &وضعیت, 0);

	إذا(0 == وضعیت) اذهب main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	خروج(EXIT_FAILURE);
}
