
/*
	WELCOME!!!!
*/


/*
	DEFINITIONS
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint32_t REGISTER;		// registers are 32 bits
typedef uint32_t WORD;			// words are 32 bits
typedef uint8_t BYTE;			// bytes are 8 bits

REGISTER registers[16];			// Registers R0-R15
WORD memory[1024];			// 4 kilobytes of RAM in this example (since 1 word = 4 bytes)
BYTE status_register = 0;		// Current program status register (Because in ARM it has a unique register.

#define COND_CODE_POS (28)
#define OPCODE_POS (21) // is it 24 or 21??

//condition code. REFERENCE :- http://www.davespace.co.uk/arm/introduction-to-arm/conditional.html
#define CC_EQ	(0x0) // Equals
#define CC_NE	(0x1) // not equal
#define CC_CS	(0x2) // Carry set (also used for Unsigned higher or same)
#define CC_CC	(0x3) // Carry clear (also used for Unsigned lower)
#define CC_MI	(0x4) // Minus / negative
#define CC_PL	(0x5) // Plus / positive
#define CC_VS	(0x6) // Overflow
#define CC_VC	(0x7) // No overflow
#define CC_HI	(0x8) // Unsigned hiher
#define CC_LS	(0x9) // Unsigned lower or same
#define CC_GE	(0xa) // Signed greater than or quals
#define CC_LT	(0xb) // Signed less than
#define CC_GT	(0xc) // Signed greater than
#define CC_LE	(0xd) // Signed less than or equals
#define CC_AL	(0xe) // Always (default)
#define CC_NV	(0xf) //DEPRECATED DO NOT USE!!! (Originally used as an analogue for the AL condictin code)
/*
These would normally only be used with jumps in other languages, but arm allows them to be used elsewhere.
Why? And inline conditions explained

Take this example:
    CMP   r3, #0
    BEQ   next
    ADD   r0, r0, r1
    SUB   r0, r0, r2
next

The above example DOES NOT have inline conditions. We just use a jump to skip them. Compare this to the below example:

    CMP   r3, #0
    ADDNE r0, r0, r1
    SUBNE r0, r0, r2

The above example DOES use inline conditions. Notice the NE after add. 
*/

#define OP_AND	(0x0)
#define OP_EOR	(0x1)
#define OP_SUB	(0x2)
#define OP_RSB	(0x3)
#define OP_ADD	(0x4)
#define OP_ADC	(0x5)
#define OP_SBC	(0x6)
#define OP_RSC	(0x7)
#define OP_TST	(0x8)
#define OP_TEQ	(0x9)
#define OP_CMP	(0xa)
#define OP_CMN	(0xb)
#define OP_ORR	(0xc)
#define OP_MOV	(0xd)
#define OP_BIC	(0xe)
#define OP_MVN	(0xf)



// status flag
#define STAT_N	(1<<7) // Result is Negative
#define STAT_Z	(1<<6) // Result is zero
#define STAT_C	(1<<5) // Set if the result is bigger than 32 bits. Also set from the output of shifter, move and logical instructions.
#define STAT_V	(1<<4) // If the operation produces a signed result bigger than 31 bits. 

/*
	FETCH/MAIN FUNCTION
*/
/*
What does each instruction line mean? We get them in hex but what does everything mean?

https://i.stack.imgur.com/0cPOx.png <- Look at this

https://armconverter.com/ <- Converter here 

WE ARE USING ARM BIG ENDIAN.

*/

int main(void) // CONSIDER THIS THE 'FETCH' PHASE
{
	int done = 0;
	registers[15] = 0;	// init the PC.
	
	// TEMP JUST FOR TESTING!!!!!!!!!!!!!!!
	memory[0] = 0xE3A00001;				// MOV r0,#1    		1110 0011 1010 0000 0000 0000 0000 0001
	memory[1] = 0xE3A01002;				// MOV r1,#2			1110 0011 1010 0000 0001 0000 0000 0010
	memory[2] = 0xE0802001;				// ADD r2,r0,r1			1110 0000 1000 0000 0010 0000 0000 0001
	memory[3] = 0xE2822001;				// ADD r2,r2,#5			1110 0010 1000 0010 0010 0000 0000 0101
	memory[4] = 0;					// at the moment this will terminate the main loop (see comments below)
	
	while (!done)
	{
		// Grab the program counter and relivent instruction
		REGISTER pc = registers[15];
		WORD instruction = memory[pc++];
		registers[15] = pc;		
		
		if ( instruction != 0 ) 
		{
			execute(instruction);
		}
		else 
		{
			done = 1;
		}
	}
	return 0;
}


/*
	EXECUTE UTILITIES
*/

/*
<operation> - A three-letter mnemonic, e.g. MOV or ADD.

{cond} - An optional two-letter condition code, e.g. EQ or CS.

{flags} - An optional additional flags. e.g. S.

Rd - The destination register.

Rn - The first source register.

Operand2 - A flexible second operand. 
*/
void execute(WORD inst)
{
	int condition_code = inst >> COND_CODE_POS;	// get the condition flags (top 4 bits of the instruction).
	int opcode = (inst >> OPCODE_POS) & 0xf;		// Grabs the opcode. 
	int exec = decode(condition_code);
	int RN;
	int RD;
	int Operand1;
	BYTE Operand2; // this is 8 bits long, we pre-defined byte datatye.
	int immediateReg;
	int target;
	
	printf("Condition Code is %d , ", condition_code);
	printf("opcode is %d \n", opcode);
	if (exec)
	{
		switch (opcode)
		{
			case OP_AND:
				break;
			case OP_EOR:
				break;
			case OP_SUB:
				break;
			case OP_ADD:
				printf("\nADD\n");
					RN = (inst >> 16) & 0xf; // First operand (ALWAYS a register)
					Operand1 = registers[RN];
					RD = (inst >> 12) & 0xf; // Desination register
					Operand2 = (inst >> 23) & 0xf; // Can be a register OR a value 
					immediateReg = (inst >> 25) & 0xf; // Use this to check if operand2 is an immediate value (1) or register(0).
					if (!immediateReg)
					{
						printf("GRABBING THE ALTERNATIVE REGISTER VALUE");
						target = (inst >> 0) & 0xf;
						Operand2 = registers[ target ];
					}
					printf("\n Adding together %d from register %d and %d into register %d \n", Operand1, RN, Operand2, RD);
					doAdd(RD,Operand1,Operand2,0);
					break;
			case OP_ADC:
				break;
			case OP_SBC:
				break;
			case OP_RSC:
				break;
			case OP_TST:
				break;
			case OP_TEQ:
				break;
			case OP_CMP:
				break;
			case OP_CMN:
				break;
			case OP_ORR:
				break;
			case OP_MOV:
				printf("\nMOVE\n");
				RD = (inst >> 12) & 0xf; 
				Operand2 = (inst >> 0) & 0xf; 
				immediateReg = (inst >> 25) & 0xf;
				if (!immediateReg)
				{
					printf("GRABBING THE ALTERNATIVE REGISTER VALUE FIX THIS");
					target = (inst >> 0) & 0xf;
					Operand2 = registers[ target ];
				}
				printf("\n Moving %d into register %d", Operand2, RD);
				doMove(RD, Operand2, 0);
				break;
			case OP_BIC:
				break;
			case OP_MVN:
				break;
		}
		printf("\n---------------------------------------\n");
	}
}
/*
	OPCODE / INSTRUCTION UTILITIES
*/

void doAdd(int regNumber, WORD op1Value, WORD op2Value, int setSR)
{
	WORD result = op1Value + op2Value;
	printf("Result is %d", result);

	registers[ regNumber ] = result;

	if ( setSR ) {
		// set the SR flags by examining the result. This can be difficult for certain flags!

		// some are simple, e.g. to set or clear the Zero flag, could use -
		if ( result == 0 ) {

			setFlag(STAT_Z);
		}
		else {

			clearFlag(STAT_Z);
		}
	}
}

void doMove(int regNumber, WORD op2Value, int setSR)
{
	registers[ regNumber ] = op2Value;
}

/*
	DECODE UTILITIES
*/
int decode(int condition_code)
{
	switch ( condition_code ) {
		/* For this I just followed the data sheet. Should be right
		
		#define STAT_N	(1<<7)
		#define STAT_Z	(1<<6) 
		#define STAT_C	(1<<5) 
		#define STAT_V	(1<<4) 
		
		*/
		case CC_EQ: 	
			return isSet( STAT_Z ) ;
			break;	// check if zero flag is set
		case CC_NE: 
			return isClear( STAT_Z ) ;
			break;	// check if zero flag is clear
		case CC_CS:// Carry set (also used for Unsigned higher or same)
			return isSet(STAT_C);
			break;
		case CC_CC:// Carry clear (also used for Unsigned lower)
			return isClear( STAT_C );
			break;
		case CC_MI:// Minus / negative
			return isSet( STAT_N );
			break;
		case CC_PL:// Plus / positive
			return isClear( STAT_N );
			break;
		case CC_VS:// Overflow
			return isSet( STAT_V );
			break;
		case CC_VC:// No overflow
			return isClear( STAT_N );
			break;
		case CC_HI:// Unsigned hiher
			return (isClear(STAT_Z) && isSet(STAT_C) );
			break;
		case CC_LS:// Unsigned lower or same
			return (isSet(STAT_Z) && isClear(STAT_C) );
			break;
		case CC_GE:// Signed greater than or quals
			return ( (isClear(STAT_N) && isClear(STAT_V) || (isSet(STAT_N) && isClear(STAT_V)) );
			break;
		case CC_LT:  //less than
			return ( isSet( STAT_N ) && isClear( STAT_V ) ) || ( isClear(STAT_N) && isSet(STAT_V) ) ;
			break;
		case CC_GT: // Signed greater than
			return ( (isClear( STAT_N ) && isClear( STAT_Z ) && isClear( STAT_V ) ) || ( (isSet( STAT_N ) && isClear( STAT_Z ) && isSet( STAT_V ) ) ;
			break;
		case CC_LE: // Signed less than or equals
			return ( isSet( STAT_N ) && isClear( STAT_V ) ) || ( isClear(STAT_N) && isSet(STAT_V) || (isSet(STAT_Z))) ;
			break;
		case CC_AL: //ALWAYS
			return 1; 
			break;
		case CC_NV: //NEVER
			return 0; 
			break;
	}
}


/*
	CURRENT PROGRAM STATUS REGISTER UTILITIES
*/

int isSet(int flag)
{
	return (status_register & flag);
}

int isClear(int flag)
{
	return ((~status_register) & flag);
}

void setFlag(int flag)
{
	status_register = status_register | flag;
}

void clearFlag(int flag)
{
	status_register = status_register & (~flag);
}

/*

*/
