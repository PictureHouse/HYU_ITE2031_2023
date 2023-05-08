/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

char labelList[1 << 16][7];
int count = 0;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int typeR(char *opcode, char *regA, char *regB, char *destReg);
int typeI(char *opcode, char *regA, char *regB, char *offsetField, int line);
int typeJ(char *opcode, char *regA, char *regB);
int typeO(char *opcode);
int fill(char *opcode, char *valueOrAddress);
int labelCheck(char *label, int count);

int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}
	
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		if (strlen(label) > 6) {
			printf("error: label length is over 6\n");
			exit(1);
		}

		if (isNumber(&label[0])) {
			printf("error: label must start with a letter\n");
			exit(1);
		}

		if (!strcmp(label, "")) {
			count++;
			continue;
		}
		else {
			if (labelCheck(label, count) != -1) {
				printf("error: use duplicated definition of label\n");
				exit(1);
			}
			strcpy(labelList[count], label);
			count++;
		}
	}

	rewind(inFilePtr);

	int line = 0;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		int machineCode = 0;

		if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
			machineCode = typeR(opcode, arg0, arg1, arg2);
		}
		else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
			machineCode = typeI(opcode, arg0, arg1, arg2, line);
		}
		else if (!strcmp(opcode, "jalr")) {
			machineCode = typeJ(opcode, arg0, arg1);
		}
		else if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop")) {
			machineCode = typeO(opcode);
		}
		else if (!strcmp(opcode, ".fill")) {
			machineCode = fill(opcode, arg0);
		}
		else {
			printf("error: unrecognized opcode\n");
			exit(1);
		}

		fprintf(outFilePtr, "%d\n", machineCode);
		printf("(address %d): %d (hex 0x%x)\n", line, machineCode, machineCode);
		line++;
	}

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	exit(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

/*
 * R-type instructions (add, nor):
 * bits 24-22: opcode
 * bits 21-19: reg A
 * bits 18-16: reg B
 * bits 15-3:  unused (should all be 0)
 * bits 2-0:   destReg
*/
int typeR(char *opcode, char *regA, char *regB, char *destReg)
{
	int binary = 0;
	int tmp = 0;

	if (!strcmp(opcode, "add")) {
		binary = 0 << 22;
	}
	else if (!strcmp(opcode, "nor")) {
		binary = 1 << 22;
	}
	else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	if (atoi(regA) < 0 || atoi(regA) > 7) {
		printf("error: outside of the register range\n");
		exit(1);
	}
	tmp = atoi(regA) << 19;
	binary = binary | tmp;

	if (atoi(regB) < 0 || atoi(regB) > 7) {
		printf("error: outside of the register range\n");
		exit(1);
	}
	tmp = atoi(regB) << 16;
	binary = binary | tmp;

	if (atoi(destReg) < 0 || atoi(destReg) > 7) {
		printf("error: outside of the register range\n");
		exit(1);
	}
	tmp = atoi(destReg);
	binary = binary | tmp;

	return binary;
}

/*
 * I-type instructions (lw, sw, beq):
 * bits 24-22: opcode
 * bits 21-19: reg A
 * bits 18-16: reg B
 * bits 15-0:  offsetField (a 16-bit value with a range of -32768 to 32767)
*/
int typeI(char *opcode, char *regA, char *regB, char *offsetField, int line)
{
	int binary = 0;
	int tmp = 0;

	if (!strcmp(opcode, "lw")) {
		binary = 2 << 22;
	}
	else if (!strcmp(opcode, "sw")) {
		binary = 3 << 22;
	}
	else if (!strcmp(opcode, "beq")) {
		binary = 4 << 22;
	}
	else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	if (atoi(regA) < 0 || atoi(regA) > 7) {
		printf("error: outside of the register range\n");
		exit(1);
	}
	tmp = atoi(regA) << 19;
	binary = binary | tmp;

	if (atoi(regB) < 0 || atoi(regB) > 7) {
		printf("error: outside of the register range\n");
		exit(1);
	}
	tmp = atoi(regB) << 16;
	binary = binary | tmp;

	if (isNumber(offsetField)) {
		tmp = atoi(offsetField);
	}
	else {
		tmp = labelCheck(offsetField, count);
		if (tmp == -1) {
			printf("error: use undefined label\n");
			exit(1);
		}
		if (!strcmp(opcode, "beq")) {
			tmp = tmp - line - 1;
		}
	}
	if (tmp < -32768 || tmp > 32767) {
		printf("error: offset out of range\n");
		exit(1);
	}
	tmp = tmp & 0xFFFF;
	binary = binary | tmp;

	return binary;
}

/*
 * J-type instructions (jalr):
 * bits 24-22: opcode
 * bits 21-19: reg A
 * bits 18-16: reg B
 * bits 15-0:  unused (should all be 0)
*/
int typeJ(char *opcode, char *regA, char *regB)
{
	int binary = 0;
	int tmp = 0;

	if (!strcmp(opcode, "jalr")) {
		binary = 5 << 22;
	}
	else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	if (atoi(regA) < 0 || atoi(regA) > 7) {
		printf("error: outside of the register range\n");
		exit(1);
	}
	tmp = atoi(regA) << 19;
	binary = binary | tmp;

	if (atoi(regB) < 0 || atoi(regB) > 7) {
		printf("error: outside of the register range\n");
		exit(1);
	}
	tmp = atoi(regB) << 16;
	binary = binary | tmp;

	return binary;
}

/*
 * O-type instructions (halt, noop):
 * bits 24-22: opcode
 * bits 21-0:  unused (should all be 0)
*/
int typeO(char *opcode)
{
	int binary = 0;

	if (!strcmp(opcode, "halt")) {
		binary = 6 << 22;
	}
	else if (!strcmp(opcode, "noop")) {
		binary = 7 << 22;
	}
	else {
		printf("error: unrecognized opcode\n");
		exit(1);
	}

	return binary;
}

int fill(char *opcode, char *valueOrAddress) 
{
	int offset = 0;
	
	if (isNumber(valueOrAddress)) {
		offset = atoi(valueOrAddress);
	}
	else {
		offset = labelCheck(valueOrAddress, count);
		if (offset == -1) {
			printf("error: use undefined label\n");
			exit(1);
		}
	}

	if (offset < -32768 || offset > 32767) {
		printf("error: offset out of range\n");
		exit(1);
	}

	return offset;
}

int labelCheck(char *label, int count)
{
	for (int i = 0; i < count; i++) {
		if (!strcmp(labelList[i], label)) {
			return i;
		}
	}
	return -1;
}
