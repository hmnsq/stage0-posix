/* Copyright (C) 2016-2021 Jeremiah Orians
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

#समावेश <stdlib.h>
#समावेश <stdio.h>

पूर्णांक मुख्य (पूर्णांक argc, अक्षर** argv)
{
	अगर(argc != 2)
	{
		ख_माला_दो("xeh $filename is the only valid input form\n", मानक_त्रुटि);
		निकास(निकास_त्रुटि);
	}

	खाता* in = ख_खोलो(argv[1], "r");
	अगर(शून्य == in)
	{
		ख_माला_दो("Unable to open file: ", मानक_त्रुटि);
		ख_माला_दो(argv[1], मानक_त्रुटि);
		ख_माला_दो(" aborting to prevent problems\n", मानक_त्रुटि);
		निकास(निकास_त्रुटि);
	}

	अक्षर* table = "0123456789ABCDEF";
	पूर्णांक c = ख_अक्षर_लो(in);
	पूर्णांक col = 40;
	जबतक(खातापूर्ण != c)
	{
		ख_अक्षर_दो(table[c / 16], मानक_निकास);
		ख_अक्षर_दो(table[c % 16], मानक_निकास);
		col = col - 2;
		अगर(0 == col)
		{
			col = 40;
			ख_अक्षर_दो('\n', मानक_निकास);
		}
		अन्यथा ख_अक्षर_दो(' ', मानक_निकास);
		c = ख_अक्षर_लो(in);
	}
	ख_अक्षर_दो('\n', मानक_निकास);
	निकास(निकास_सफल);
}
