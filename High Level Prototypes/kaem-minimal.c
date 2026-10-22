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

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <sys/wait.h>

#ವ್ಯಾಖ್ಯಾನಿಸು FALSE 0
#ವ್ಯಾಖ್ಯಾನಿಸು TRUE 1
#ವ್ಯಾಖ್ಯಾನಿಸು max_string 4096
#ವ್ಯಾಖ್ಯಾನಿಸು max_ವಾದಗಳು 256

ಅಕ್ಷರ** ಟೋಕನುಗಳು;
ಪೂರ್ಣಾಂಕ command_ಮುಗಿದ;

/* Function for purging line comments */
ಶೂನ್ಯ collect_comment(FILE* ಒಳಹರಿವು)
{
	ಪೂರ್ಣಾಂಕ c;
	ಮಾಡು
	{
		c = fgetc(ಒಳಹರಿವು);
		ಆದರೆ(EOF == c) ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	} ಆಗ('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
ಪೂರ್ಣಾಂಕ collect_string(FILE* ಒಳಹರಿವು, ಪೂರ್ಣಾಂಕ index, ಅಕ್ಷರ* ಗುರಿ)
{
	ಪೂರ್ಣಾಂಕ c;
collect_string_loop:
	c = fgetc(ಒಳಹರಿವು);
	ಆದರೆ(EOF == c) ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ('"' == c) ಹಿಂತಿರುಗಿ index;
	ಗುರಿ[index] = c;
	index = index + 1;
	ಹೋಗು collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
ಅಕ್ಷರ* collect_token(FILE* ಒಳಹರಿವು)
{
	ಅಕ್ಷರ* token = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(max_string, ಗಾತ್ರ(ಅಕ್ಷರ));
	ಪೂರ್ಣಾಂಕ c;
	ಪೂರ್ಣಾಂಕ i = 0;
collect_token_loop:
	c = fgetc(ಒಳಹರಿವು);
	ಆದರೆ(EOF == c) ನಿರ್ಗಮಿಸು(EXIT_SUCCESS);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((' ' == c) || ('\t' == c)) ಹೋಗು collect_token_ಮುಗಿದ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ('\n' == c)
	{ /* Command terminates at end of line */
		command_ಮುಗಿದ = 1;
		ಹೋಗು collect_token_ಮುಗಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(ಒಳಹರಿವು, i, token);
		ಹೋಗು collect_token_ಮುಗಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(ಒಳಹರಿವು);
		command_ಮುಗಿದ = 1;
		ಹೋಗು collect_token_ಮುಗಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(ಒಳಹರಿವು);
		ಹೋಗು collect_token_ಮುಗಿದ;
	}
	token[i] = c;
	i = i + 1;
	ಹೋಗು collect_token_loop;

collect_token_ಮುಗಿದ:
	ಆದರೆ(0 == i) ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ token;
}

ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ** argv, ಅಕ್ಷರ** envp)
{
	ಅಕ್ಷರ* filename = "kaem.run";
	FILE* script = ಖಾಲಿ;

	ಆದರೆ(ಖಾಲಿ != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	ಆದರೆ(ಖಾಲಿ == script) ನಿರ್ಗಮಿಸು(EXIT_FAILURE);

main_loop:
	ಟೋಕನುಗಳು = ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(max_ವಾದಗಳು, ಗಾತ್ರ(ಅಕ್ಷರ*));

	ಪೂರ್ಣಾಂಕ i = 0;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = 0;
	command_ಮುಗಿದ = 0;
	ಮಾಡು
	{
		ಅಕ್ಷರ* ಫಲಿತಾಂಶ = collect_token(script);
		ಆದರೆ(0 != ಫಲಿತಾಂಶ)
		{ /* Not a comment string but an actual argument */
			ಟೋಕನುಗಳು[i] = ಫಲಿತಾಂಶ;
			i = i + 1;
		}
	} ಆಗ(0 == command_ಮುಗಿದ);

	ಆದರೆ(0 > i) ಹೋಗು main_loop;

	fputs(" +> ", stdout);
	ಪೂರ್ಣಾಂಕ j;
	ಗಾಗಿ(j = 0; j < i; j = j + 1)
	{
		fputs(ಟೋಕನುಗಳು[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	ಅಕ್ಷರ* ಕಾರ್ಯಕ್ರಮ = ಟೋಕನುಗಳು[0];
	ಆದರೆ(ಖಾಲಿ == ಕಾರ್ಯಕ್ರಮ) ನಿರ್ಗಮಿಸು(EXIT_FAILURE);

	ಪೂರ್ಣಾಂಕ f = fork();
	ಆದರೆ (f == -1) ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f == 0)
	{ /*ಮಗು*/
		/* execve() returns only on error */
		execve(ಕಾರ್ಯಕ್ರಮ, ಟೋಕನುಗಳು, envp);
		/* Prevent infinite loops */
		ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &ಸ್ಥಿತಿ_2, 0);

	ಆದರೆ(0 == ಸ್ಥಿತಿ_2) ಹೋಗು main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	ನಿರ್ಗಮಿಸು(EXIT_FAILURE);
}
