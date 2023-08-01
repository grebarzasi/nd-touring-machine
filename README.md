# nd-touring-machine

An interpreter for non-deterministic Turing Machines with a single tape, using the standard C language with only the libc library. The input file structure consists of the transition function, followed by the accepting states, a maximum limit on the number of steps to perform a single computation (to avoid the problem of machines that do not terminate), and finally a series of strings to be read by the machine. 
The output is expected to be a file containing 0 for the non-accepted strings and 1 for the accepted ones; since there is also a limit on the number of steps, the result can also be U if acceptance is not reached.

The file input is divided into four parts:
* The transition function
* The accepting states
* The maximum number of steps
* The list of strings to be processed by the machine

The project uses "_" as the blank symbol on the tape, "L", "R", and "S" as the movement commands for the tape head, and assumes that the tape is unlimited on both sides and contains "_" in every position. The input file is provided through the standard input, while the output file is on the standard output.
