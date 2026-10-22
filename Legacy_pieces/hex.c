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

#അന്തര്ഭാവ<stdio.h>
#അന്തര്ഭാവ<stdlib.h>
#അന്തര്ഭാവ<stdbool.h>

ശൂന്യം purge_വരി_comments()
{
	പൂർണ്ണസംഖ്യ c;
	ചെയ്യുക
	{
		c = getchar();

		എങ്കിൽ (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} ആകുമ്പോൾ ( '\n' != c );
}

പൂർണ്ണസംഖ്യ hex(പൂർണ്ണസംഖ്യ c)
{
	എങ്കിൽ (c >= '0' && c <= '9')
	{
		തിരികെ (c - 48);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c >= 'a' && c <= 'z')
	{
		തിരികെ (c - 87);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c >= 'A' && c <= 'Z')
	{
		തിരികെ (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
	പൂർണ്ണസംഖ്യ c;
	പൂർണ്ണസംഖ്യ sum = 0;
	bool toggle;
	toggle = false;

	ചെയ്യുക
	{
		c = getchar();
		എങ്കിൽ ((c == '#') || (c == ';')) purge_വരി_comments();
		അല്ലെങ്കിൽ എങ്കിൽ ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			എങ്കിൽ(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			അല്ലെങ്കിൽ
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}ആകുമ്പോൾ(c != EOF);

	exit(EXIT_SUCCESS);
}
