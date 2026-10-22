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

#وصف_ڪريو FALSE 0
#وصف_ڪريو TRUE 1
#وصف_ڪريو max_string 4096
#وصف_ڪريو max_دليلون 256

حرف** ٽوڪن;
صحيح command_پورو;

/* Function for purging line comments */
فراغ collect_comment(FILE* داخلا)
{
	صحيح c;
	افعل
	{
		c = fgetc(داخلا);
		إذا(EOF == c) ٻاهر_نڪرو(EXIT_FAILURE);
	} طالما('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
صحيح collect_string(FILE* داخلا, صحيح index, حرف* هدف)
{
	صحيح c;
collect_string_loop:
	c = fgetc(داخلا);
	إذا(EOF == c) ٻاهر_نڪرو(EXIT_FAILURE);
	وإلا إذا('"' == c) أرجع index;
	هدف[index] = c;
	index = index + 1;
	اذهب collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
حرف* collect_token(FILE* داخلا)
{
	حرف* token = صفر_يادگيري_ڏيو(max_string, حجم(حرف));
	صحيح c;
	صحيح i = 0;
collect_token_loop:
	c = fgetc(داخلا);
	إذا(EOF == c) ٻاهر_نڪرو(EXIT_SUCCESS);
	وإلا إذا((' ' == c) || ('\t' == c)) اذهب collect_token_پورو;
	وإلا إذا('\n' == c)
	{ /* Command terminates at end of line */
		command_پورو = 1;
		اذهب collect_token_پورو;
	}
	وإلا إذا('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(داخلا, i, token);
		اذهب collect_token_پورو;
	}
	وإلا إذا('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(داخلا);
		command_پورو = 1;
		اذهب collect_token_پورو;
	}
	وإلا إذا('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(داخلا);
		اذهب collect_token_پورو;
	}
	token[i] = c;
	i = i + 1;
	اذهب collect_token_loop;

collect_token_پورو:
	إذا(0 == i) أرجع عدم;
	أرجع token;
}

صحيح مکيه(صحيح argc, حرف** argv, حرف** envp)
{
	حرف* filename = "kaem.run";
	FILE* script = عدم;

	إذا(عدم != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	إذا(عدم == script) ٻاهر_نڪرو(EXIT_FAILURE);

main_loop:
	ٽوڪن = صفر_يادگيري_ڏيو(max_دليلون, حجم(حرف*));

	صحيح i = 0;
	صحيح حالت = 0;
	command_پورو = 0;
	افعل
	{
		حرف* نتيجو = collect_token(script);
		إذا(0 != نتيجو)
		{ /* Not a comment string but an actual argument */
			ٽوڪن[i] = نتيجو;
			i = i + 1;
		}
	} طالما(0 == command_پورو);

	إذا(0 > i) اذهب main_loop;

	fputs(" +> ", stdout);
	صحيح j;
	لكل(j = 0; j < i; j = j + 1)
	{
		fputs(ٽوڪن[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	حرف* پروگرام = ٽوڪن[0];
	إذا(عدم == پروگرام) ٻاهر_نڪرو(EXIT_FAILURE);

	صحيح f = fork();
	إذا (f == -1) ٻاهر_نڪرو(EXIT_FAILURE);
	وإلا إذا (f == 0)
	{ /*ٻار*/
		/* execve() returns only on error */
		execve(پروگرام, ٽوڪن, envp);
		/* Prevent infinite loops */
		ٻاهر_نڪرو(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &حالت, 0);

	إذا(0 == حالت) اذهب main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	ٻاهر_نڪرو(EXIT_FAILURE);
}
