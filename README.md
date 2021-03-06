# SAP-1_Emulator
 A simple emulator for a modified version of the SAP-1 processor. Allows for reading .sap files that work as a basic low-level assembly language.

### Emulator
The SAP-1 emulator is a C++ class made up of several stages.

First, a file cotaining SAP-1 assembly code is loaded. This can either be set through a constructor when initialising the class object or changed after it was defined with the 
``` C++
bool openFile(std::string _codeFile)
``` 
function call. `_codefile` can be either a path to the `.sap` file or just the name if the code is in the same directory as the project files.

Next, an autoparser reads the contents of the ```.sap``` file and loads any instructions into the RAM. Once parsing is done, the sequencer automatically starts, program counter begins incrementing thus running any commands on the RAM, until it reaches a ```HLT``` command.

------
### Instruction set

The modified SAP-1 emulator contains all off the original instructions, their respective opcodes and mnemonics, as well as an additional `MOV` instruction for storing variables in a given memory address:

| Mnemonic | Opcode   | Arguments        | Description |
| -------- | -------- | ---------------- | ----------- |
| MOV      | `0b11`   | `MEM ADDR` & `VALUE` | Stores `VALUE` in the RAM at the address `MEM ADDR`. |
| LDA      | `0b0`    | `MEM ADDR`           | Loads a value from the given address into the accumulator (Reg A) |
| ADD      | `0b1`    | `MEM ADDR`           | Loads a value from the given address into the register B, adds it to whatever is in the accumulator (Reg A) via the ALU and returns sum back into Reg A. |
| SUB      | `0b10`   | `MEM ADDR`           | Loads a value from the given address into the register B, subtracts it from whatever is in the accumulator (Reg A) via the ALU and returns value back into Reg A. |
| OUT      | `0b1110` | None                 | Displays whatever is in the accumulator (Reg A) via the 7 segment display emulator. |
| HLT      | `0b1111` | None                 | Halts the clock and ends the program. Must be at the end of every `.sap` file, otherwise the rpogram counter will loop forever. |

Variables can be defined anywhere in any order, even after they're referenced in the code, because the code file gets parsed, turned into machine code, and stored in the RAM before it is executed.

Variables may only be refered to by pointers (their address in the memory). Variables cannot have names. **Variables may not be stored in the addresses already used for storing instructions**. It is advised to declare variables at the end of the `.sap` file and point them to free memory addresses.

Because the RAM uses 4bit memory addresses, you may only have a total of 16 instruction calls and variables put together.

------
### RAM

The RAM is a 16 word, 8 bit array. In other words, you can store up to 16 8 bit values. The RAM is addressed to by the MAR (Memory Address Register) which takes input from the 4 least significant bits of the instruction register (Reg C).

------
### `.sap` files

Traditionally, SAP-1 is programmed by hand, by changing the address the MAR is pointing to and manually loading the instruction into the RAM. To speed up this rather tedious process, this modified version of the SAP-1 emulator reads its instructions from a file.

For SAP-1.cpp a `.sap` file is an ordinary text file with a changed extension, so it'll still work if you feed it a `.txt`.

For SAP-Args.cpp a file ending in `.sap` must be provided.

------
#### A sample program
##### sampleProgram.sap 

```txt
LDA 1010
ADD 1011
OUT
HLT
MOV 1010 00000001
MOV 1011 00000010
```
##### Output
``` txt
 _
 _|
 _|

Reg A:
03
ALU:
03
Reg B:
02
Reg C:
f0
BUS:
f0
PC:
04
RAM:
0a 1b e0 f0
00 00 00 00
00 00 01 02
00 00 00 00
```
Based on the state of
```C++
SAPcpu::bool bin;
```
the program may output in binary or hex.	

### SAP-1.cpp & SAP-Args.cpp

SAP-1.cpp is an example program that uses a console interface with custom commands to control the emulator. By default, the CPU is pointing to a default `samlpeProgram.sap` example program. Typing `RUN` in the console will execute any loaded `.sap` file.

SAP-1.cpp only works on windows, SAP-Args only works on Linux.

SAP-Args.cpp must take a `.sap` file argument with `-x [FILE NAME.sap]. It does not initialise a default file otherwise.

SAP-Args.cpp must also know what base the registers should be printed in, which is supplied with -b2 or -b16 for binary or hexadecimal respectively.


For additional commans, run `HELP` in the console or `-h` if using SAP-Args.cpp.
