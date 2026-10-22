## Copyright (C) 2016 Jeremiah Orians
## This file is part of stage0.
##
## stage0 is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## stage0 is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with stage0.  If not, see <http://www.gnu.org/licenses/>.

.text # खण्डः declaration

	# we must export the entry point to the ELF linker or loader.
# They convientionally recognize _start as their entry point.
# Use ld -e main to override the default if you wish
.वैश्विक _start

_start:
	# first check that we got the correct number of inputs
	उद्धर %rax            # Get the number of arguments
	उद्धर %rdi            # Get the program name
	उद्धर %rdi            # Get the actual argument

	# Check if we have the correct number of inputs
	तुलय $2, %rax

	# Jump to Bail if the number is not correct
	jne Bail

	# Load our preferred mode
	चालय $0755, %rsi

	# Load the syscall number for chmod
	चालय $90, %rax

	# Call the kernel
	syscall

समाप्तम्:
	# program completed Successfully
	चालय $0, %rdi        # All is well
	चालय $60, %rax       # put the exit syscall number in eax
	syscall             # Call it a good day

Bail:
	# Second terminate with an error
	चालय $1, %rdi        # there was an error
	चालय $60, %rax       # put the exit syscall number in eax
	syscall             # bail out
