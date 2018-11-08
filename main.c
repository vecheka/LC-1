#include <stdio.h>
#include "lc-1.h"
#define ADD 1
#define LD 2
#define ST 3
#define AND 5
#define LDR 6
#define NOT 9
#define JMP 12
#define LEA 14
#define HALT 0


/**
Load memory's data from a text file.
@param lc LC class object
*/

void loadMemory(LC *lc) {

	FILE *file;

	file = fopen("memory.txt", "r");

	char hex[7];

	int i = 0;

	while(fscanf(file, "%s", hex) != EOF) {
		lc->memory[i++] = (Register) strtol(hex, NULL, 16);
	}

	fclose(file);

}

/**
Initialize the register with some data.
@param lc LC class object
*/
void initializeRegister(LC *lc) {
  int i;
  for (i = 0; i < NO_OF_REGISTERS; i++) lc->cpus.reg_file[i] = i;
}

/**
Print the state of the current machine.
@param lc LC class object
*/
void printCurrentStateOfMachine(LC *lc) {
  int i;
  for (i = 0; i < NO_OF_REGISTERS; i++) {
    printf("Register #%d = %X\n", i, lc->cpus.reg_file[i]);
  }
  printf("IR = 0x%X\n", lc->cpus.IR);
  printf("PC = 0x%X\n", lc->cpus.PC);
  printf("SEXT = 0x%X\n", lc->cpus.SEXT);
  printf("MAR = 0x%X\n", lc->cpus.MAR);
  printf("MDR = 0x%X\n", lc->cpus.MDR);

}


/**
Execute the program
@param lc LC class object
*/
int controller(LC *lc) {

	unsigned int opcode, Dr, Sr1, Sr2, immed5, immed9, currentPC;
  Register ir, a, b, r;

	int state = FETCH;
  lc->cpus.PC = 0;
  
  for (;;) {
    switch(state) {

      case FETCH:
        printf("Here in FETCH\n");
        currentPC = lc->cpus.PC;
        lc->cpus.IR = lc->memory[lc->cpus.PC++];
        ir = lc->cpus.IR;
        // printf("%X\n", getOffset9(ir));
        state = DECODE;
        printCurrentStateOfMachine(lc);
        break;

      case DECODE:
        printf("Here in DECODE\n");

        // get opcode
        opcode = getOpcode(lc->cpus.IR);

        state = EVAL_ADDR;
        printCurrentStateOfMachine(lc);
        break;

      case EVAL_ADDR:

        printf("Here in EVAL_ADDR\n");
        switch(opcode) {
          case ADD:
            // Bit[5] = 0 -> DR <= SR1 + SR2
            // Bit[5] = 1 -> DR <= SR1 + immed5 
            Dr = getDr(ir);
            Sr1 = getSr1(ir);
            if (isBitFiveOne(ir)) lc->cpus.SEXT = getOffset6(ir);
            else Sr2 = getSr2(ir);
            break;
          case AND:
            // Bit[5] = 0 -> DR <= SR1 & SR2
            // Bit[5] = 1 -> DR <= SR1 & immed5
            Dr = getDr(ir);
            Sr1 = getSr1(ir);
            if (isBitFiveOne(ir)) lc->cpus.SEXT = getOffset6(ir);
            else Sr2 = getSr2(ir);
            break;
          case JMP:
            // PC <= BaseR (SR1)
            Sr1 = getSr1(ir);
            break;
          case LD:
            // DR <= Mem[PC + offset9]
            Dr = getDr(ir);
            lc->cpus.SEXT = getOffset9(ir);
            break;
          case LDR:
            // DR <= BaseR (SR1) + immed5
            Dr = getDr(ir);
            Sr1 = getSr1(ir);
            lc->cpus.SEXT = getOffset6(ir);
            break;
          case LEA:
            // DR <= PC + offset9
            Dr = getDr(ir);
            lc->cpus.SEXT = getOffset9(ir);
            break;
          case NOT:
            // DR <= NOT(SR1)
            Dr = getDr(ir);
            Sr1 = getSr1(ir);
            break;
          case ST:
            // Mem[offset9] <= SR(DR)
            Dr = getDr(ir);
            lc->cpus.SEXT = getOffset9(ir);
            break;
          case HALT:
            exit(0);
            break;
        }

        printCurrentStateOfMachine(lc);
        state = FETCH_OP;

        break;

      case FETCH_OP:

        printf("Here in FETCH_OP\n");
        
        switch(opcode) {
          case ADD:
            // Bit[5] = 0 -> DR <= SR1 + SR2
            // Bit[5] = 1 -> DR <= SR1 + immed5 
            if (isBitFiveOne(ir)) {
              a = lc->cpus.reg_file[Sr1];
              immed5 = lc->cpus.SEXT;
            } else {
              a = lc->cpus.reg_file[Sr1];
              b = lc->cpus.reg_file[Sr2];
            }
            break;
          case AND:
            // Bit[5] = 0 -> DR <= SR1 & SR2
            // Bit[5] = 1 -> DR <= SR1 & immed5
            if (isBitFiveOne(ir)) {
              a = lc->cpus.reg_file[Sr1];
              immed5 = lc->cpus.SEXT;
            } else {
              a = lc->cpus.reg_file[Sr1];
              b = lc->cpus.reg_file[Sr2];
            }
            break;
          case JMP:
            // PC <= BaseR (SR1)
            a = lc->cpus.reg_file[Sr1];
            break;
          case LD:
            // DR <= Mem[PC + offset9]
            immed9 = lc->cpus.SEXT;
            break;
          case LDR:
            // DR <= BaseR (SR1) + immed5
            a = lc->cpus.reg_file[Sr1];
            immed5 = lc->cpus.SEXT;
            break; 
          case LEA:
            // DR <= PC + offset9
            immed9 = lc->cpus.SEXT;
            break;
          case NOT:
            // DR <= NOT(SR1)
            a = lc->cpus.reg_file[Sr1];
            break;
          case ST:
            // Mem[offset9] <= SR(DR)
            immed9 = lc->cpus.SEXT;
            a = lc->cpus.reg_file[Dr];
            break;
          case HALT:
            exit(0);
            break;
        }

        printCurrentStateOfMachine(lc);
        state = EXECUTE;

        break;

      case EXECUTE:

        printf("Here in EXECUTE\n");
        switch(opcode) {
        case ADD:
          // Bit[5] = 0 -> DR <= SR1 + SR2
          // Bit[5] = 1 -> DR <= SR1 + immed5 
          if (isBitFiveOne(ir)) {
            r = a + immed5;
          } else {
            r = a + b;
          }
          break;
        case AND:
          // Bit[5] = 0 -> DR <= SR1 & SR2
          // Bit[5] = 1 -> DR <= SR1 & immed5
          if (isBitFiveOne(ir)) {
            r = a & immed5;
          } else {
            r = a & b;
          }
          break;
        case JMP:
          // PC <= BaseR (SR1)
          r = a;
          break;
        case LD:
          // DR <= Mem[PC + offset9]
          lc->cpus.MAR = currentPC + immed9;
          r = lc->memory[currentPC + immed9];
          break;
        case LDR:
          // DR <= Mem[BaseR (SR1) + immed5]
          lc->cpus.MAR = a + immed5;
          r = lc->memory[a + immed5];
          break;
        case LEA:
          // DR <= PC + offset9
          r = currentPC + immed9;
          break;
        case NOT:
          // DR <= NOT(SR1)
          r = !(a);
          break;
        case ST:
          // Mem[offset9] <= SR(DR)
          r = a;
          break;
        case HALT:
          exit(0);
          break;
      }

      printCurrentStateOfMachine(lc);
      state = STORE;
      break;

      case STORE:

        printf("Here in STORE\n");

        switch(opcode) {
          case ADD:
            // Bit[5] = 0 -> DR <= SR1 + SR2
            // Bit[5] = 1 -> DR <= SR1 + immed5 
            lc->cpus.reg_file[Dr] = r;
            break;
          case AND:
            // Bit[5] = 0 -> DR <= SR1 & SR2
            // Bit[5] = 1 -> DR <= SR1 & immed5
            lc->cpus.reg_file[Dr] = r;
            break;
          case JMP:
            // PC <= BaseR (SR1)
            lc->cpus.PC = r;
            break;
          case LD:
            // DR <= Mem[PC + offset9]
            lc->cpus.MDR = r;
            lc->cpus.reg_file[Dr] = r;
            break;
          case LDR:
            // DR <= BaseR (SR1) + immed5
            lc->cpus.MDR = r;
            lc->cpus.reg_file[Dr] = r;
            break;
          case LEA:
            // DR <= PC + offset9
            lc->cpus.reg_file[Dr] = r;
            break;
          case NOT:
            // DR <= NOT(SR1)
            lc->cpus.reg_file[Dr] = r;
            break;
          case ST:
            // Mem[offset9] <= SR(DR)
            lc->memory[immed9] = r;
            break;
          case HALT:
            exit(0);
            break;
        }

        printCurrentStateOfMachine(lc);
        state = FETCH;
        
        break;

    }
    printf("------------------------\n");

  }
  
	return 0;

}





int main(void) {
  LC *lc = malloc(sizeof(LC));
  loadMemory(lc);
  initializeRegister(lc);
  controller(lc);
  // free(lc);
  return 0;
}