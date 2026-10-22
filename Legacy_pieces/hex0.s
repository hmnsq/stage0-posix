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

# Our writable space
.data
.वैश्विक _start
hex:
	# Purge Comment Lines
	तुलय $35, %rax
	je purge_comment
	# deal all ascii less than 0
	तुलय $48, %rax
	jl ascii_other
	# deal with 0-9
	तुलय $58, %rax
	jl ascii_num
	# deal with all ascii less than A
	तुलय $65, %rax
	jl ascii_other
	# deal with A-F
	तुलय $71, %rax
	jl ascii_high
	# deal with all ascii less than a
	तुलय $97, %rax
	jl ascii_other
	# deal with a-f
	तुलय $103, %rax
	jl ascii_low
	# The rest that remains needs to be ignored
	कूर्द ascii_other

purge_comment:
	# Attempt to read 1 byte from STDIN
	चालय $1, %rdx          # set the size of chars we want
	चालय $निवेशः, %rsi      # Where to put it
	चालय $0, %rdi          # Where are we reading from
	चालय $0, %rax          # the syscall number for read
	syscall               # आह्वान the Kernel

	test %rax, %rax       # check what we got
	jz समाप्तम्               # Got EOF आह्वान it done

	# load byte
	movb निवेशः, %al       # load char
	movzx %al, %rax       # We have to zero extend it to use it

	# Loop if not LF
	तुलय $10, %rax
	jne purge_comment

	# Otherwise return -1
	चालय $-1, %rax
	प्रत्यागच्छ

ascii_num:
	वियोजय $48, %rax
	प्रत्यागच्छ
ascii_low:
	वियोजय $87, %rax
	प्रत्यागच्छ
ascii_high:
	वियोजय $55, %rax
	प्रत्यागच्छ
ascii_other:
	चालय $-1, %rax
	प्रत्यागच्छ

_start:
	# Our flag for byte processing
	चालय $-1, %r15

	# temp storage for the sum
	चालय $0, %r14

loop:
	# Attempt to read 1 byte from STDIN
	चालय $1, %rdx          # set the size of chars we want
	चालय $निवेशः, %rsi      # Where to put it
	चालय $0, %rdi          # Where are we reading from
	चालय $0, %rax          # the syscall number for read
	syscall               # आह्वान the Kernel

	test %rax, %rax       # check what we got
	jz समाप्तम्               # Got EOF आह्वान it done

	# load byte
	movb निवेशः, %al       # load char
	movzx %al, %rax       # We have to zero extend it to use it

	# process byte
	आह्वान hex

	# deal with -1 values
	तुलय $0, %rax
	jl loop

	# deal with toggle
	तुलय $0, %r15
	jge print

	# process first byte of pair
	चालय %rax, %r14
	चालय $0, %r15
	कूर्द loop

# process second byte of pair
print:
	# update the sum and store in output
	shl $4, %r14
	योजय %r14, %rax
	चालय %al, output

	# flip the toggle
	चालय $-1, %r15

	# Print our first Hex
	चालय $1, %rdx          # set the size of chars we want
	चालय $output, %rsi     # What we are writing
	चालय $1, %rdi          # Stdout File Descriptor
	चालय $1, %rax          # the syscall number for write
	syscall               # आह्वान the Kernel

	कूर्द loop

समाप्तम्:
	# program completed Successfully
	चालय $0, %rdi          # All is well
	चालय $60, %rax         # put the exit syscall number in eax
	syscall               # Call it a good day

read_size = 2
निवेशः:
	.byte read_size
output:
	.byte 0x00
