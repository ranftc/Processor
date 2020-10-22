# Processor

Date Start:
Date End:

Language: C

Required to run program: 
- Understanding the assigned Instruction Set requirements and exectuions.
- mem_in.txt set of hexadecimal opcode values.
- mem_out.txt is what the output file should look like. It's used for verification.

Problem: To design a simple processor based on the simplified 2-step instruction cycle. You will be creating two functions in software, one to simulate the fetch next instruction cycle, fetchNextInstruction(), and one to simulate the execute instruction cycle, executeInstruction(). Neither of these functions will have parameters nor will they return values. They will operation on global data meant to simulate the registers and memory of the processor. This simulated machine consists of four registers that will be represented in your software with four global variables.
- PC -- Program counter (16 bit) used to hold the address of the next instruction to
execute. It is initialized to zero.
- IR -- Instruction register (8 bit) used to hold the current instruction being executed.
- MAR -- Memory Address Register (16 bit) used to hold an address being used as a pointer, i.e., an indirect reference to data in memory.
- ACC -- Accumulator (8 bit) used to operate on data.

Memory will be simulated with an array memory[65536]. The function fetchNextInstruction() will perform the following steps:
- Use the program counter (PC) as an index to retrieve an instruction from the array memory[].
- Store the retrieved instruction to the instruction register (IR).
- Determine from the instruction how much to increment PC so that it points to the next instruction in memory[] and perform the increment. 

The function executeInstruction() will examine IR to determine which operation to perform and
it will perform it on the data contained in the registers and in the array memory[]

The Instruction Set (OPCODE) values are provided and detailed in the attached pdf

Solution: Created a processor to take a text file input of hexadecimal values deliminated by space and then process the opcode based of the instruction. It will then execute the instruction and print it out as hexadecimals in a text file.
