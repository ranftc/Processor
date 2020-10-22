/*
Name of program: decoder.c (HW_2)
Your Name: Casey Ranft
The Due Date: 2/2/18
This program takes a Hexadecimal and decodes it
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <Opcode.c>

//Stop Code
#define HALT_OPCODE 0x19

//No Operation Code
#define NOP_OPCODE 0b00011000

//MASKs
#define SourceMASK 0b10000011
#define OperationMASK 0b11110000
#define DestinationMASK 0b10001100
#define FunctionMASK 0b00001000
#define RegisterMASK 0b00000100
#define MethodMASK 0b00000011
#define BranchMASK 0b00010111
#define MultiMASK 256

//Prototypes
unsigned char hexToChar(char);
char charToHex(unsigned char);
void fetchNextInstruction(void);
void executeInstruction(void);
unsigned int getInt(void);

unsigned char memory[65536];
unsigned char ACC = 0;
unsigned char IR = 0;
unsigned int MAR = 0;
unsigned int PC = 0;
bool CONT = true;


int main(){

	FILE * fileIn;
	FILE * fileOut;
	char tempHex[2];

	fileIn = fopen("mem_in.txt","r");
	//fileIn = fopen("testMemIn.txt","r");

	int count = 0;
	while(fscanf(fileIn, "%s", tempHex) == 1){
		unsigned char binary = (hexToChar(tempHex[0]) * 16) + hexToChar(tempHex[1]);
		memory[count] = binary;
		count++;
	}

	//Execution loop. Continue fetching and executing
	//until PC point to HALT instruction
	while(CONT){
		fetchNextInstruction();
		executeInstruction();
	}

	char finalHex[2];
	fileOut = fopen("mem_out_test.txt","w");
	//fileOut = fopen("test_out.txt","w");
	for (int i = 1; i < sizeof(memory); i++){
		finalHex[0] = charToHex(memory[i-1]/ 16); //Left-Side of binary
		finalHex[1] = charToHex(memory[i-1] & 0b00001111); //Right-Side of binary
		fwrite(finalHex, 1, sizeof(finalHex), fileOut);
		fprintf(fileOut, " ");
		if(i%16 == 0){
			fprintf(fileOut, "\n");
		}
	}
	//printf("ACC = %0.2X\n",ACC);
	//printf("MAR = %0.4X\n",MAR);
	//printf("PC = %d\n",PC);
	printf("It ran all the way through");

	return 0;
}

void fetchNextInstruction()
{
	IR = memory[PC];
	if(IR != HALT_OPCODE){
		if(IR != NOP_OPCODE){ //NOP

			if((IR & 0b10000000) == 0b10000000){
				//Destination
				unsigned int destID = 5;
				unsigned char DEST = (DestinationMASK & IR);
				switch(DEST){
				case 0b10000100 : //ACC
					destID = 0;
					break;
				case 0b10001000 : //MAR
					destID = 1;
					break;
				case 0b10001100 : //MEM
					destID = 2;
					break;
				}

				//Source
				unsigned char IND = (SourceMASK & IR);
				switch(IND){
				case 0b10000000 : //IND
					if(destID == 2){
						PC = PC + 3;
					}else{
						PC++;
					}
					break;
				case 0b10000001 : //ACC
					if(destID == 2){
						PC = PC + 3;
					}else{
						PC++;
					}
					break;
				case 0b10000010 : //CON
					if(destID == 1){ //16bit Operand
						PC = PC + 3;
					}else{ //8 bit operand
						PC = PC + 2;
					}
					break;
				case 0b10000011 : //MEM - 16bit Operand
					PC = PC + 3;
				}
			}else if ((IR & 0b11110000) == 0b00000000){

				//Register
				unsigned int reg = 3;
				unsigned char REG = (RegisterMASK & IR);
				switch(REG){
				case 0b00000000 : //ACC
					reg = 0;
					break;
				case 0b00000100 : //Index reg. MAR
					reg = 1;
					break;
				}

				//STOR/LOAD
				unsigned char funType = 3;
				unsigned char FUN = (FunctionMASK & IR);
				switch(FUN){
				case 0b00000000 : //STOR
					funType = 0;
					break;
				case 0b00001000 : //LOAD
					funType = 1;
					break;
				}

				//Method
				unsigned char METHOD = (MethodMASK & IR);
				switch(METHOD){
				case 0b00000000 : //16bit Operand used as address
					PC = PC + 3;
					break;
				case 0b00000001 : //Operand used as constant
					if(reg == 0 && funType == 0){ //Write ACC into Constant
						//This doesn't make sense
						CONT = false;
						printf("Write ACC into Constant isn't a thing\n");
					}else if(reg == 1 && funType == 0){ //Write MAR into Constant
						//Not needed
						CONT = false;
						printf("Write MAR into Constant isn't a thing\n");
					}else if(reg == 0 && funType == 1){ //LOAD ACC with 8bit Constant
						PC = PC + 2;
					}else if(reg == 1 && funType == 1){ //LOAD MAR with 16bit Constant
						PC =  PC + 3;
					}
					break;
				case 0b00000010 : //IND
					PC++;
				}
				//Branch is always followed by 16bits
			}else if((IR & 0b00010000) == 0b00010000){
				PC = PC + 3;
			}
		}else{
			PC++;
		}
	}else{
		CONT = false;
	}
}

void executeInstruction()
{
	if(IR != HALT_OPCODE){
		if(IR != NOP_OPCODE) {//NOP

			//Mathematical or Logical
			if((IR & 0b10000000) == 0b10000000){
				//Destination
				unsigned int destinationValue = 0;
				unsigned int destID = 3;
				unsigned char DEST = DestinationMASK & IR;
				switch(DEST){
				case 0b10000000 : //IND
					destinationValue = memory[MAR];
					break;
				case 0b10000100 : //ACC
					destinationValue = ACC;
					destID = 0;
					break;
				case 0b10001000 : //MAR
					destinationValue = MAR;
					destID = 1;
					break;
				case 0b10001100 : //MEM
					destinationValue = memory[getInt()];
					break;
				}

				//Source
				unsigned int sourceValue = 0;
				unsigned char IND = SourceMASK & IR;
				switch(IND){
				case 0b10000000 : //IND
					sourceValue = memory[MAR];
					break;
				case 0b10000001 : //ACC
					sourceValue = ACC;
					break;
				case 0b10000010 : //CON
					if(destID == 1){ //16bit operand
						sourceValue = getInt();
					}else{ //8 bit operand
						sourceValue = memory[PC-1];
					}
					break;
				case 0b10000011 : //MEM
					sourceValue = memory[getInt()];
				}

				//Operation
				unsigned char operationValue = 0;
				unsigned char AND = OperationMASK & IR;
				switch(AND){
				case 0b10000000 : //AND
					operationValue = sourceValue & destinationValue;
					break;
				case 0b10010000 : //OR
					operationValue = sourceValue | destinationValue;
					break;
				case 0b10100000 : //XOR
					operationValue = sourceValue ^ destinationValue;
					break;
				case 0b10110000 : //ADD
					operationValue = sourceValue + destinationValue;
					break;
				case 0b11000000 : //SUB
					operationValue = sourceValue - destinationValue;
					break;
				case 0b11010000 : //INC
					operationValue = ++destinationValue;
					break;
				case 0b11100000 : //DEC
					operationValue = --destinationValue;
					break;
				case 0b11110000 : //NOT
					operationValue = ~destinationValue;
				}

				//Writing to Destination
				switch(DEST){
				case 0b10000000 : //IND
					memory[MAR] = operationValue;
					break;
				case 0b10000100 : //ACC
					ACC = operationValue;
					break;
				case 0b10001000 : //MAR
					MAR = operationValue;
					break;
				case 0b10001100 : //MEM
					memory[getInt()] = operationValue;
					break;
				}
				//Memory Operations
			}else if((IR & 0b11110000) == 0b0000000){

				//Register
				unsigned int reg = 3;
				unsigned char REG = RegisterMASK & IR;
				switch(REG){
				case 0b00000000 : //ACC
					reg = 0;
					break;
				case 0b00000100 : //Index reg. MAR
					reg = 1;
					break;
				}

				//STOR/LOAD
				unsigned char funType = 3;
				unsigned char FUN = FunctionMASK & IR;
				switch(FUN){
				case 0b00000000 : //STOR
					funType = 0;
					break;
				case 0b00001000 : //LOAD
					funType = 1;
					break;
				}

				//Method
				unsigned char METHOD = MethodMASK & IR;
				switch(METHOD){
				case 0b00000000 : //16bit Operand used as address
					if(reg == 0 && funType == 0){ //Write ACC into memory[16bits]
						memory[getInt()] = ACC;
					}else if(reg == 1 && funType == 0){ //Write MAR into memory[16bits]
						memory[getInt()] = MAR/256;
						memory[getInt() + 1] = MAR & 0x00FF;
					}else if(reg == 0 && funType == 1){ //LOAD ACC with memory[16bit]
						ACC = memory[getInt()];
					}else if(reg == 1 && funType == 1){ //LOAD MAR with memory[16bit]
						MAR = (memory[getInt()] * MultiMASK) + memory[getInt() + 1];
					}
					break;
				case 0b00000001 : //Operand used as constant
					if(reg == 0 && funType == 0){ //Write ACC into Constant
						//This doesn't make sense
					}else if(reg == 1 && funType == 0){ //Write MAR into Constant
						//Not needed
					}else if(reg == 0 && funType == 1){ //LOAD ACC with 8bit Constant
						ACC = memory[PC-1];
					}else if(reg == 1 && funType == 1){ //LOAD MAR with 16bit Constant
						MAR = getInt();
					}
					break;
				case 0b00000010 : //IND
					if(reg == 0 && funType == 0){ //Write ACC into memory[MAR]
						memory[MAR] = ACC;
					}else if (reg == 1 && funType == 0){ //Write MAR into mem[MAR]
						memory[MAR] = MAR/256;
						memory[MAR+1] = MAR & 0x00FF;
					}else if (reg == 0 && funType == 1){ //LOAD ACC with mem[MAR]
						ACC = memory[MAR];
					}else if (reg == 1 && funType == 1){ //LOAD MAR with mem[MAR]
						MAR = memory[MAR] * MultiMASK + memory[MAR + 1];
					}
				}
			}else if((IR & 0b00010000) == 0b00010000){

				//Branch
				unsigned char branch = BranchMASK & IR;
				switch(branch){
				case 0b00010000 : //BRA
					PC = getInt();
					break;
				case 0b00010001 : //BRZ
					if(ACC == 0){
						PC = getInt();
					}
					break;
				case 0b00010010 : //BNE
					if(ACC != 0){
						PC = getInt();
					}
					break;
				case 0b00010011 : //BLT
					if(ACC < 0){
						PC = getInt();
					}
					break;
				case 0b00010100 : //BLE
					if(ACC <= 0){
						PC = getInt();
					}
					break;
				case 0b00010101 : //BGT
					if(ACC > 0){
						PC = getInt();
					}
					break;
				case 0b00010110 : //BGE
					if(ACC >= 0){
						PC = getInt();
					}
					break;
				}
			}
		}
	}else{
		CONT = false;
	}
}

unsigned int getInt(void){
	unsigned int value = (memory[PC-2] * MultiMASK) + memory[PC-1];
	return value;
}
unsigned char hexToChar(char hex){
	unsigned char digit = 0;
	if( hex >= '0' && hex <= '9')
	{
		digit = hex - '0';

	}
	else if (hex >= 'A' && hex <= 'F'){
		digit = hex - 'A' + 10;
	}
	else
	{
		digit = hex - 'a' + 10;
	}
	return digit;
}

char charToHex(unsigned char binary){
	char hexDigit = 0;
	if(binary >= 0 && binary <= 9 ){
		hexDigit = binary + '0';
	}else{
		hexDigit  = binary + 'a' - 10;
	}
	return hexDigit;
}
