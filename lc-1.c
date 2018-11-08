#include <stdio.h>
#include "lc-1.h"

/**
Get opcode from the IR
@param theIR instruction register
@return opcode
*/
int getOpcode(Register theIR) {
  return (theIR & 0xF000) >> (HEX_BITS - CODE_BITS);
}

/**
Get Destination Register from the IR
@param theIR instruction register
@return Destination Register
*/
int getDr(Register theIR) {
  return (theIR & 0x0F00) >> (HEX_BITS - (CODE_BITS * 2) + 1);
}


/**
Get source register 1 from the IR
@param theIR instruction register
@return source register 1
*/
int getSr1(Register theIR) {
  return (theIR & 0x01C0) >> (CODE_BITS + 2);
}

/**
Get source register 2 when bit[5] = 1 from the IR
@param theIR instruction register
@return source register 1
*/
int getSr2(Register theIR) {
  return (theIR & 0x0007) >> 0;
}

/**
Check if bit 5 is 1 or 0
@param theIR instruction register
@return 1 if bit 5 is 1
*/
int isBitFiveOne(Register theIR) {
  return (theIR & 0x0020) >> (CODE_BITS + 1);
}


/**
Get offset 6 from the IR
@param theIR instruction register
@return immed5
*/
int getOffset6(Register theIR) {
  return isBitFiveOne(theIR) == 1 ? (theIR & 0x003F) | 0xFF00 : (theIR & 0x003F);
}


/**
Get offset 9 from the IR
@param theIR instruction register
@return immed8
*/
int getOffset9(Register theIR) {
  int offset9 = (theIR & 0x01FF);
  if ((offset9 & 0x0100) >> (CODE_BITS * 2) == 1) return offset9 | 0XFF00;
  return offset9; 
}