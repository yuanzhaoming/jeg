#ifndef NES_H
#define NES_H

#include <stdint.h>

#include "ppu.h"
#include "cpu6502.h"

typedef uint16_t (*controller_read_func_t) (void); // read controller data [controller1, controller2] with [Left, Right, Up, Down, Start, Select, A, B]

typedef struct nes_t {
  cpu6502_t cpu;
  ppu_t ppu;
  controller_read_func_t controller_read;
} nes_t;

void nes_init(nes_t *nes, ppu_update_frame_func_t update_frame, controller_read_func_t controller_read);

void nes_reset(nes_t *nes);

void nes_load_rom(nes_t *nes, char *filename);

void nes_iterate_frame(nes_t *nes); // run cpu until next complete frame

void nes_store_state(nes_t *nes, uint8_t *data); // stores state to given array
void nes_load_state(nes_t *nes, uint8_t *data); // load state to given array

#endif
