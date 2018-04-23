#ifndef PPU_H
#define PPU_H

#include <stdint.h>

typedef struct nes_t nes_t;

typedef int (*ppu_read_func_t) (nes_t *nes, int address); // read data [8bit] from address [16bit]
typedef void (*ppu_write_func_t) (nes_t *nes, int address, int value); // write data [8bit] to address [16bit]
typedef void (*ppu_update_frame_func_t) (void *reference, uint8_t* frame_data, int width, int height);

typedef struct ppu_t {
  nes_t *nes; // reference to nes console

  // ppu state
  uint64_t last_cycle_number; // measured in cpu cycles
  int cycle;
  int scanline;

  uint_fast8_t palette[32];
  uint8_t name_table[2048];
  uint8_t oam_data[256];

  // ppu registers
  int v; // current vram address (15bit)
  int t; // temporary vram address (15bit)
  int x; // fine x scoll (3bit)
  int w; // toggle bit (1bit)
  int f; // even/odd frame flag (1bit)

  int register_data;

  // background temporary variables
  int name_table_byte;
  int attribute_table_byte;
  int low_tile_byte;
  int high_tile_byte;
  uint64_t tile_data;

  // sprite temporary variables
  int sprite_count;
  uint32_t sprite_patterns[8];
  uint_fast8_t sprite_positions[8];
  uint_fast8_t sprite_priorities[8];
  uint_fast8_t sprite_indicies[8];

  // memory accessable registers
  int ppuctrl;
  int ppumask;
  int ppustatus;
  int oam_address;
  int buffered_data;
   
  // memory interface to vram and vrom
  ppu_read_func_t read;
  ppu_write_func_t write;

  // frame data interface
  uint8_t *video_frame_data;
} ppu_t;

void ppu_init(ppu_t *ppu, nes_t *nes, ppu_read_func_t read, ppu_write_func_t write);
void ppu_setup_video(ppu_t *ppu, uint8_t *video_frame_data);

void ppu_reset(ppu_t *ppu);

//! \brief read data [8bit] from address [16bit]
extern uint_fast8_t ppu_read(ppu_t *ppu, uint_fast16_t hwAddress) ; 

//! \brief write data [8bit] to address [16bit]
extern void ppu_write(ppu_t *ppu, uint_fast16_t hwAddress, uint_fast8_t chData); 

//! \bridef dedicated PPU DMA access 
extern void ppu_dma_access(ppu_t *ppu, uint_fast8_t chData);

extern int ppu_update(ppu_t *ppu); // update ppu to current cpu cycle, return number of cpu cycles to next frame

#endif
