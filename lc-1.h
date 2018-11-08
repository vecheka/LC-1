#include <stdio.h>
#include<stdlib.h>
#ifndef LC_H
#define LC_H

#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5


#define CODE_BITS 4
#define HEX_BITS 16
#define MEM_SIZE 32
#define NO_OF_REGISTERS 8
typedef unsigned short Register;

/* CPU class.*/
typedef struct cpu_s {
  Register IR, PC, SEXT, MDR, MAR;
  Register reg_file[NO_OF_REGISTERS];
} CPU_s;

/* LC_1 class*/
typedef struct lc {
  CPU_s cpus;
  Register memory[MEM_SIZE];
} LC;



/** Functions Declarations.*/
int getOpcode(Register);
int getDr(Register);
int getSr1(Register);
int getSr2(Register);
int isBitFiveOne(Register);
int getOffset6(Register);
int getOffset9(Register);




#endif