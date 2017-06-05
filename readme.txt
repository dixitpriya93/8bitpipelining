
This is a program written in C -program and expects an input.txt file which has REEGISTERS,Memory and code information
and simulate a MIPS Compiler with Stall logic, branch predict not taken, Data bypassing and forwarding and
Detects NEXT: keyword only for branching.

************Compiled Using GNU GCC Compiler*******************

Instructions for execution:
--------------------------

1) place the input.txt file in working directory

2) For convenience Working directory is printed on the console after you run the program and will give option to user to place the
file in the specified location

3) output.txt file must be backed up as re running will erase the old contents



Here is the breif summary of, How code works?:
----------------------------------------------

1) Markers will be created after every keyword i.e., Registers, memory and code

2) Range of code will be sent to particular functions for processing

3) Registers function will take the markers and will fill in the registers with the values in text file using support functions

4) Same with Memory function

5) Code function will read the text file until end of file is reached and OPCODE's are defines as below

Struct instr is defined as compared to opcode in operating system.

6) code will be read in to separate instructions and will send data to program_counter 2-D array accordingly for future processing

Operation codes for various addressing modes of instructions:

LD- Displacement - 1
LD- Immediate - 2
SD- Displacement - 3
SD- Immediate - 4
DADD- Register - 5
DADD- Immediate - 6
SUB- Register- 7
SUB- Register - 8
BNEZ- 9

7) Each code line is encoded into Program_counter register and will be decoded in individual instructions

8) Main will trigger IF1 which will parallelly triggers the next instructions depending on the Instruction flag which is created
by a struct cycle_flag to know the status of every instruction

9) data Forwarding and bypassing is taken care by struct instr decoded_instruction

10) isEqual function detects if logic needs stall by comparing data in pipeline

11) Branching will take place by triggering correct pc_num value that is to the line which has NEXT: keyword, which is tracked by 

bnez_tracker array which have 0- if branching or not, 1- pc_num of instr where branching happens and 2- to destination instruction 

12) After encoding is done

13) Program enters into decoding phase

14) It uses program_counter register which have ~opcodes and decodes and triggers correct operation through the pipeline

IF1 - pc_num value is given to IF1

IF2- PC will be loaded to struct instr instruction[]

ID- Loaded values are decoded

EX- Execution phase triggers stall logic to check whether stall is required or not

MEM1

MEM2- Load, store operations are done writing/reading

MEM3

WB- Writing the decoded values to registers

Forwading is already done by decode_instruction struct node created of type instr

15) When the execution stage passes through every cycle there are some coded values which will be sent to print_output array
and print_output_function uses this array to print

Decrypted values:

12- No -op

1- IF1

2- IF2

3- ID

4- EX

5- MEM1

6- MEM2

7- MEM3

8- WB

99 - stall

this uses print_tabs and branch_tabs support functions

16) Output is printed in required format with FILE* op

17) Print_registers and print_memorys will be called to print the final values of both in to output.txt file





