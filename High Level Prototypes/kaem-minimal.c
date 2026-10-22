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

#تعریف_کړه FALSE 0
#تعریف_کړه TRUE 1
#تعریف_کړه max_string 4096
#تعریف_کړه max_دلیلونه 256

حرف** نښې;
صحيح command_بشپړ;

/* Function for purging line comments */
فراغ collect_comment(FILE* ننوت)
{
	صحيح c;
	افعل
	{
		c = fgetc(ننوت);
		إذا(EOF == c) وتل(EXIT_FAILURE);
	} طالما('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
صحيح collect_string(FILE* ننوت, صحيح index, حرف* موخه)
{
	صحيح c;
collect_string_loop:
	c = fgetc(ننوت);
	إذا(EOF == c) وتل(EXIT_FAILURE);
	وإلا إذا('"' == c) أرجع index;
	موخه[index] = c;
	index = index + 1;
	اذهب collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
حرف* collect_token(FILE* ننوت)
{
	حرف* token = حافظه_صفر_ورکړه(max_string, حجم(حرف));
	صحيح c;
	صحيح i = 0;
collect_token_loop:
	c = fgetc(ننوت);
	إذا(EOF == c) وتل(EXIT_SUCCESS);
	وإلا إذا((' ' == c) || ('\t' == c)) اذهب collect_token_بشپړ;
	وإلا إذا('\n' == c)
	{ /* Command terminates at end of line */
		command_بشپړ = 1;
		اذهب collect_token_بشپړ;
	}
	وإلا إذا('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(ننوت, i, token);
		اذهب collect_token_بشپړ;
	}
	وإلا إذا('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(ننوت);
		command_بشپړ = 1;
		اذهب collect_token_بشپړ;
	}
	وإلا إذا('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(ننوت);
		اذهب collect_token_بشپړ;
	}
	token[i] = c;
	i = i + 1;
	اذهب collect_token_loop;

collect_token_بشپړ:
	إذا(0 == i) أرجع عدم;
	أرجع token;
}

صحيح اصلي(صحيح argc, حرف** argv, حرف** envp)
{
	حرف* filename = "kaem.run";
	FILE* script = عدم;

	إذا(عدم != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	إذا(عدم == script) وتل(EXIT_FAILURE);

main_loop:
	نښې = حافظه_صفر_ورکړه(max_دلیلونه, حجم(حرف*));

	صحيح i = 0;
	صحيح حالت = 0;
	command_بشپړ = 0;
	افعل
	{
		حرف* پایله = collect_token(script);
		إذا(0 != پایله)
		{ /* Not a comment string but an actual argument */
			نښې[i] = پایله;
			i = i + 1;
		}
	} طالما(0 == command_بشپړ);

	إذا(0 > i) اذهب main_loop;

	fputs(" +> ", stdout);
	صحيح j;
	لكل(j = 0; j < i; j = j + 1)
	{
		fputs(نښې[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	حرف* پروګرام = نښې[0];
	إذا(عدم == پروګرام) وتل(EXIT_FAILURE);

	صحيح f = fork();
	إذا (f == -1) وتل(EXIT_FAILURE);
	وإلا إذا (f == 0)
	{ /*ماشوم*/
		/* execve() returns only on error */
		execve(پروګرام, نښې, envp);
		/* Prevent infinite loops */
		وتل(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &حالت, 0);

	إذا(0 == حالت) اذهب main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	وتل(EXIT_FAILURE);
}
