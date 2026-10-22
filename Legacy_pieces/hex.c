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

शून्य purge_पङ्क्ति_comments()
{
	पूर्णाङ्क c;
	गर
	{
		c = अक्षर_लो();

		यदि (खातापूर्ण == c)
		{
			निकास(निकास_सफल);
		}
	} जबसम्म ( '\n' != c );
}

पूर्णाङ्क hex(पूर्णाङ्क c)
{
	यदि (c >= '0' && c <= '9')
	{
		फर्काऊ (c - 48);
	}
	अन्यथा यदि (c >= 'a' && c <= 'z')
	{
		फर्काऊ (c - 87);
	}
	अन्यथा यदि (c >= 'A' && c <= 'Z')
	{
		फर्काऊ (c - 55);
	}

	म_लिखो("You managed to call a hex function without a hex value!!!\n");
	निकास(निकास_त्रुटि);
}

पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण *argv[])
{
	पूर्णाङ्क c;
	पूर्णाङ्क sum = 0;
	bool toggle;
	toggle = false;

	गर
	{
		c = अक्षर_लो();
		यदि ((c == '#') || (c == ';')) purge_पङ्क्ति_comments();
		अन्यथा यदि ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			यदि(!toggle)
			{
				sum = hex(c);
				toggle = true;
			}
			अन्यथा
			{
				sum = (sum << 4) + hex(c);
				toggle = false;
				ख_अक्षर_दो(sum, मानक_निकास);
			}
		}
	}जबसम्म(c != खातापूर्ण);

	निकास(निकास_सफल);
}
