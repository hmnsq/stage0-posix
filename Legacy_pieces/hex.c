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

#समावेश<stdio.h>
#समावेश<stdlib.h>
#समावेश<stdbool.h>

व्योम purge_पंक्ति_comments()
{
	पूर्णांक c;
	करो
	{
		c = अक्षर_लो();

		अगर (खातापूर्ण == c)
		{
			निकास(निकास_सफल);
		}
	} जबतक ( '\n' != c );
}

पूर्णांक hex(पूर्णांक c)
{
	अगर (c >= '0' && c <= '9')
	{
		वापस (c - 48);
	}
	अन्यथा अगर (c >= 'a' && c <= 'z')
	{
		वापस (c - 87);
	}
	अन्यथा अगर (c >= 'A' && c <= 'Z')
	{
		वापस (c - 55);
	}

	म_लिखो("You managed to call a hex function without a hex value!!!\n");
	निकास(निकास_त्रुटि);
}

पूर्णांक मुख्य(पूर्णांक argc, अक्षर *argv[])
{
	पूर्णांक c;
	पूर्णांक योग = 0;
	bool toggle;
	toggle = false;

	करो
	{
		c = अक्षर_लो();
		अगर ((c == '#') || (c == ';')) purge_पंक्ति_comments();
		अन्यथा अगर ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			अगर(!toggle)
			{
				योग = hex(c);
				toggle = true;
			}
			अन्यथा
			{
				योग = (योग << 4) + hex(c);
				toggle = false;
				ख_अक्षर_दो(योग, मानक_निकास);
			}
		}
	}जबतक(c != खातापूर्ण);

	निकास(निकास_सफल);
}
