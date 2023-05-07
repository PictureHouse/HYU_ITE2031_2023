/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;
    int count = 0;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    state.pc = 0;
    int halt = 0;
    while (!halt) {
        if (state.pc < 0 || state.pc > NUMMEMORY) {
            printf("error: program counter out of range\n");
            exit(1);
        }

        int opcode = (state.mem[state.pc] >> 22) & 0x0007;
        int arg0 = (state.mem[state.pc] >> 19) & 0x0007;
        int arg1 = (state.mem[state.pc] >> 16) & 0x0007;
        int arg2 = state.mem[state.pc] & 0xFFFF;

        printState(&state);

        /* add */
        if (opcode == 0) {
            state.reg[arg2] = state.reg[arg0] + state.reg[arg1];
        }
        /* nor */
        else if (opcode == 1) {
            state.reg[arg2] = ~(state.reg[arg0] | state.reg[arg1]);
        }
        /* lw */
        else if (opcode == 2) {
            int offset = convertNum(arg2);
            if (offset < -32768 || offset > 32767) {
                printf("error: offset out of range\n");
                exit(1);
            }
            state.reg[arg1] = state.mem[state.reg[arg0] + offset];
        }
        /* sw */
        else if (opcode == 3) {
            int offset = convertNum(arg2);
            if (offset < -32768 || offset > 32767) {
                printf("error: offset out of range\n");
                exit(1);
            }
            state.mem[state.reg[arg0] + offset] = state.reg[arg1];
        }
        /* beq */
        else if (opcode == 4) {
            int offset = convertNum(arg2);
            if (offset < -32768 || offset > 32767) {
                printf("error: offset out of range\n");
                exit(1);
            }
            if (state.reg[arg0] == state.reg[arg1]) {
                state.pc += offset;
            }
        }
        /* jalr */
        else if (opcode == 5) {
            state.reg[arg1] = state.pc + 1;
            state.pc = state.reg[arg0];
        }
        /* halt */
        else if (opcode == 6) {
            halt = 1;
        }
        /* noop */
        else if (opcode == 7) {
            halt = 0;
        }
        else {
            printf("error: unrecognized opcode\n");
            exit(1);
        }

        state.pc++;
        count++;
    }
    
    printf("machine halted\n");
    printf("total of %d instructions executed\n", count);
    printf("final state of machine:\n");
    printState(&state);

    fclose(filePtr);
    exit(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
