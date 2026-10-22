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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/wait.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FALSE 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRUE 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ max_string 4096
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ max_ਦਲੀਲਾਂ 256

ਅੱਖਰ** ਟੋਕਨ;
ਪੂਰਨ command_ਪੂਰਾ;

/* Function for purging line comments */
ਖਾਲੀ collect_comment(FILE* ਨਿਵੇਸ਼)
{
	ਪੂਰਨ c;
	ਕਰੋ
	{
		c = fgetc(ਨਿਵੇਸ਼);
		ਜੇ(EOF == c) ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	} ਜਦੋਂ('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
ਪੂਰਨ collect_string(FILE* ਨਿਵੇਸ਼, ਪੂਰਨ index, ਅੱਖਰ* ਨਿਸ਼ਾਨਾ)
{
	ਪੂਰਨ c;
collect_string_loop:
	c = fgetc(ਨਿਵੇਸ਼);
	ਜੇ(EOF == c) ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	ਨਹੀਂਤਾਂ ਜੇ('"' == c) ਵਾਪਸ index;
	ਨਿਸ਼ਾਨਾ[index] = c;
	index = index + 1;
	ਜਾਓ collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
ਅੱਖਰ* collect_token(FILE* ਨਿਵੇਸ਼)
{
	ਅੱਖਰ* token = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(max_string, ਆਕਾਰ(ਅੱਖਰ));
	ਪੂਰਨ c;
	ਪੂਰਨ i = 0;
collect_token_loop:
	c = fgetc(ਨਿਵੇਸ਼);
	ਜੇ(EOF == c) ਬਾਹਰ_ਨਿਕਲੋ(EXIT_SUCCESS);
	ਨਹੀਂਤਾਂ ਜੇ((' ' == c) || ('\t' == c)) ਜਾਓ collect_token_ਪੂਰਾ;
	ਨਹੀਂਤਾਂ ਜੇ('\n' == c)
	{ /* Command terminates at end of line */
		command_ਪੂਰਾ = 1;
		ਜਾਓ collect_token_ਪੂਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(ਨਿਵੇਸ਼, i, token);
		ਜਾਓ collect_token_ਪੂਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(ਨਿਵੇਸ਼);
		command_ਪੂਰਾ = 1;
		ਜਾਓ collect_token_ਪੂਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(ਨਿਵੇਸ਼);
		ਜਾਓ collect_token_ਪੂਰਾ;
	}
	token[i] = c;
	i = i + 1;
	ਜਾਓ collect_token_loop;

collect_token_ਪੂਰਾ:
	ਜੇ(0 == i) ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ token;
}

ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ** argv, ਅੱਖਰ** envp)
{
	ਅੱਖਰ* filename = "kaem.run";
	FILE* script = ਨਿਰਾ;

	ਜੇ(ਨਿਰਾ != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	ਜੇ(ਨਿਰਾ == script) ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);

main_loop:
	ਟੋਕਨ = ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(max_ਦਲੀਲਾਂ, ਆਕਾਰ(ਅੱਖਰ*));

	ਪੂਰਨ i = 0;
	ਪੂਰਨ ਸਥਿਤੀ = 0;
	command_ਪੂਰਾ = 0;
	ਕਰੋ
	{
		ਅੱਖਰ* ਨਤੀਜਾ = collect_token(script);
		ਜੇ(0 != ਨਤੀਜਾ)
		{ /* Not a comment string but an actual argument */
			ਟੋਕਨ[i] = ਨਤੀਜਾ;
			i = i + 1;
		}
	} ਜਦੋਂ(0 == command_ਪੂਰਾ);

	ਜੇ(0 > i) ਜਾਓ main_loop;

	fputs(" +> ", stdout);
	ਪੂਰਨ j;
	ਲਈ(j = 0; j < i; j = j + 1)
	{
		fputs(ਟੋਕਨ[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	ਅੱਖਰ* ਪ੍ਰੋਗਰਾਮ = ਟੋਕਨ[0];
	ਜੇ(ਨਿਰਾ == ਪ੍ਰੋਗਰਾਮ) ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);

	ਪੂਰਨ f = fork();
	ਜੇ (f == -1) ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	ਨਹੀਂਤਾਂ ਜੇ (f == 0)
	{ /*ਬੱਚਾ*/
		/* execve() returns only on error */
		execve(ਪ੍ਰੋਗਰਾਮ, ਟੋਕਨ, envp);
		/* Prevent infinite loops */
		ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &ਸਥਿਤੀ, 0);

	ਜੇ(0 == ਸਥਿਤੀ) ਜਾਓ main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	ਬਾਹਰ_ਨਿਕਲੋ(EXIT_FAILURE);
}
