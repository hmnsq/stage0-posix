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

#अन्तर्भाव<stdio.h>
#अन्तर्भाव<stdlib.h>
#अन्तर्भाव<stdbool.h>

शून्य purge_पङ्क्तिः_comments()
{
	पूर्णाङ्क c;
	करोहि
	{
		c = getchar();

		सचे (EOF == c)
		{
			exit(EXIT_SUCCESS);
		}
	} याव ( '\n' != c );
}

पूर्णाङ्क hex(पूर्णाङ्क c)
{
	सचे (c >= '0' && c <= '9')
	{
		पटिनिवत्त (c - 48);
	}
	अञ्ञथा सचे (c >= 'a' && c <= 'z')
	{
		पटिनिवत्त (c - 87);
	}
	अञ्ञथा सचे (c >= 'A' && c <= 'Z')
	{
		पटिनिवत्त (c - 55);
	}

	printf("You managed to call a hex function without a hex value!!!\n");
	exit(EXIT_FAILURE);
}

पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण *argv[])
{
	पूर्णाङ्क c;
	पूर्णाङ्क sum = 0;
	bool toggle;
	toggle = false;

	करोहि
	{
		c = getchar();
		सचे ((c == '#') || (c == ';')) purge_पङ्क्तिः_comments();
		अञ्ञथा सचे ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			सचे(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			अञ्ञथा
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				fputc(sum, stdout);
			}
		}
	}याव(c != EOF);

	exit(EXIT_SUCCESS);
}
