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

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <sys/wait.h>

#నిర్వచించు FALSE 0
#నిర్వచించు TRUE 1
#నిర్వచించు max_string 4096
#నిర్వచించు max_వాదనలు 256

అక్షరం** టోకెన్లు;
పూర్ణాంకం command_పూర్తి;

/* Function for purging line comments */
శూన్య collect_comment(FILE* ఆదానం)
{
	పూర్ణాంకం c;
	చేయి
	{
		c = fgetc(ఆదానం);
		ఒకవేళ(EOF == c) నిష్క్రమించు(EXIT_FAILURE);
	} అయితే('\n' != c);
}

/* Function for collecting RAW strings and removing the " that goes with them */
పూర్ణాంకం collect_string(FILE* ఆదానం, పూర్ణాంకం index, అక్షరం* లక్ష్యం)
{
	పూర్ణాంకం c;
collect_string_loop:
	c = fgetc(ఆదానం);
	ఒకవేళ(EOF == c) నిష్క్రమించు(EXIT_FAILURE);
	లేకపోతే ఒకవేళ('"' == c) తిరిగి index;
	లక్ష్యం[index] = c;
	index = index + 1;
	వెళ్ళు collect_string_loop;
}

/* Function to collect an individual argument or purge a comment */
అక్షరం* collect_token(FILE* ఆదానం)
{
	అక్షరం* token = సున్నా_జ్ఞాపకం_ఇవ్వు(max_string, పరిమాణం(అక్షరం));
	పూర్ణాంకం c;
	పూర్ణాంకం i = 0;
collect_token_loop:
	c = fgetc(ఆదానం);
	ఒకవేళ(EOF == c) నిష్క్రమించు(EXIT_SUCCESS);
	లేకపోతే ఒకవేళ((' ' == c) || ('\t' == c)) వెళ్ళు collect_token_పూర్తి;
	లేకపోతే ఒకవేళ('\n' == c)
	{ /* Command terminates at end of line */
		command_పూర్తి = 1;
		వెళ్ళు collect_token_పూర్తి;
	}
	లేకపోతే ఒకవేళ('"' == c)
	{ /* RAW strings are everything between a pair of "" */
		i = collect_string(ఆదానం, i, token);
		వెళ్ళు collect_token_పూర్తి;
	}
	లేకపోతే ఒకవేళ('#' == c)
	{ /* Line comments to aid the humans */
		collect_comment(ఆదానం);
		command_పూర్తి = 1;
		వెళ్ళు collect_token_పూర్తి;
	}
	లేకపోతే ఒకవేళ('\\' == c)
	{ /* Support for end of line escapes, drops the char after */
		fgetc(ఆదానం);
		వెళ్ళు collect_token_పూర్తి;
	}
	token[i] = c;
	i = i + 1;
	వెళ్ళు collect_token_loop;

collect_token_పూర్తి:
	ఒకవేళ(0 == i) తిరిగి ఖాళీ;
	తిరిగి token;
}

పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం** argv, అక్షరం** envp)
{
	అక్షరం* filename = "kaem.run";
	FILE* script = ఖాళీ;

	ఒకవేళ(ఖాళీ != argv[1])
	{
		filename = argv[1];
	}

	script = fopen(filename, "r");

	ఒకవేళ(ఖాళీ == script) నిష్క్రమించు(EXIT_FAILURE);

main_loop:
	టోకెన్లు = సున్నా_జ్ఞాపకం_ఇవ్వు(max_వాదనలు, పరిమాణం(అక్షరం*));

	పూర్ణాంకం i = 0;
	పూర్ణాంకం స్థితి_2 = 0;
	command_పూర్తి = 0;
	చేయి
	{
		అక్షరం* ఫలితం = collect_token(script);
		ఒకవేళ(0 != ఫలితం)
		{ /* Not a comment string but an actual argument */
			టోకెన్లు[i] = ఫలితం;
			i = i + 1;
		}
	} అయితే(0 == command_పూర్తి);

	ఒకవేళ(0 > i) వెళ్ళు main_loop;

	fputs(" +> ", stdout);
	పూర్ణాంకం j;
	కొరకు(j = 0; j < i; j = j + 1)
	{
		fputs(టోకెన్లు[j], stdout);
		fputc(' ', stdout);
	}
	fputs("\n", stdout);

	అక్షరం* కార్యక్రమం = టోకెన్లు[0];
	ఒకవేళ(ఖాళీ == కార్యక్రమం) నిష్క్రమించు(EXIT_FAILURE);

	పూర్ణాంకం f = fork();
	ఒకవేళ (f == -1) నిష్క్రమించు(EXIT_FAILURE);
	లేకపోతే ఒకవేళ (f == 0)
	{ /*బిడ్డ*/
		/* execve() returns only on error */
		execve(కార్యక్రమం, టోకెన్లు, envp);
		/* Prevent infinite loops */
		నిష్క్రమించు(EXIT_FAILURE);
	}

	/* Otherwise we are the parent */
	/* And we should wait for it to complete */
	waitpid(f, &స్థితి_2, 0);

	ఒకవేళ(0 == స్థితి_2) వెళ్ళు main_loop;

	/* Clearly the script hit an issue that should never have happened */
	fputs("Subprocess error\nABORTING HARD\n", stderr);
	/* stop to prevent damage */
	నిష్క్రమించు(EXIT_FAILURE);
}
