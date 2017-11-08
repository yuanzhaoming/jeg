#include "cpu6502_debug.h"
#include "cpu6502_opcodes.h"
#include "cpu6502.h"
#include <stdio.h>

void cpu6502_dump(cpu6502_t *cpu) {
  struct opcode_tbl_entry opcode=opcode_tbl[cpu->read(cpu->reference, cpu->reg_PC)];

  printf("%8ld A=$%02X X=$%02X Y=$%02X SP=$%02X PC=$%04X ", cpu->cycle_number, cpu->reg_A, cpu->reg_X, cpu->reg_Y, cpu->reg_SP, cpu->reg_PC);
  printf("%c%c%c%c", cpu->status_N?'N':'n', cpu->status_V?'V':'v', cpu->status_U?'U':'u', cpu->status_B?'B':'b');
  printf("%c%c%c%c ", cpu->status_D?'D':'d', cpu->status_I?'I':'i', cpu->status_Z?'Z':'z', cpu->status_C?'C':'c');
  printf("%s ", opcode.mnemonic);
  switch(opcode.address_mode) {
    case ADR_ABSOLUTE:
      printf("$%02X%02X", cpu->read(cpu->reference, cpu->reg_PC+2), cpu->read(cpu->reference, cpu->reg_PC+1));
      break;
    case ADR_ABSOLUTE_X:
      printf("$%02X%02X,X", cpu->read(cpu->reference, cpu->reg_PC+2), cpu->read(cpu->reference, cpu->reg_PC+1));
      break;
    case ADR_ABSOLUTE_Y:
      printf("$%02X%02X,Y", cpu->read(cpu->reference, cpu->reg_PC+2), cpu->read(cpu->reference, cpu->reg_PC+1));
      break;
    case ADR_ACCUMULATOR:
      printf("A");
      break;
    case ADR_IMMEDIATE:
      printf("#$%02X", cpu->read(cpu->reference, cpu->reg_PC+1));
      break;
    case ADR_INDEXED_INDIRECT:
      printf("($%02X,X)", cpu->read(cpu->reference, cpu->reg_PC+1));
      break;
    case ADR_INDIRECT:
      printf("($%02X%02X)", cpu->read(cpu->reference, cpu->reg_PC+2), cpu->read(cpu->reference, cpu->reg_PC+1));
      break;
    case ADR_INDIRECT_INDEXED:
      printf("($%02X),Y", cpu->read(cpu->reference, cpu->reg_PC+1));
      break;
    case ADR_RELATIVE:
      if (cpu->read(cpu->reference, cpu->reg_PC+2)&0x80) {
        printf("$%04X", cpu->reg_PC-(256-(cpu->read(cpu->reference, cpu->reg_PC+1)+(cpu->read(cpu->reference, cpu->reg_PC+2)<<8))));
      }
      else {
        printf("$%04X", cpu->reg_PC+(cpu->read(cpu->reference, cpu->reg_PC+1)+(cpu->read(cpu->reference, cpu->reg_PC+2)<<8)));
      }
      break;
    case ADR_ZERO_PAGE:
      printf("$%02X ; which is #$%02X", cpu->read(cpu->reference, cpu->reg_PC+1), cpu->read(cpu->reference, cpu->read(cpu->reference, cpu->reg_PC+1)));
      break;
    case ADR_ZERO_PAGE_X:
      printf("$%02X,X ; which is #$%02X", cpu->read(cpu->reference, cpu->reg_PC+1), cpu->read(cpu->reference, (cpu->read(cpu->reference, cpu->reg_PC+1)+cpu->reg_X)&0xFF));
      break;
    case ADR_ZERO_PAGE_Y:
      printf("$%02X,Y ; which is #$%02X", cpu->read(cpu->reference, cpu->reg_PC+1), cpu->read(cpu->reference, (cpu->read(cpu->reference, cpu->reg_PC+1)+cpu->reg_Y)&0xFF));
      break;
  }
  printf("\n");
}
