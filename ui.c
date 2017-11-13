#include <stdio.h>
#include <SDL.h>
#include "nes.h"

// global variables
SDL_Surface *screen; // used in main and update_frame
uint16_t controller_data=0; // used in main and controller_read

typedef struct rgb_entry_t {uint8_t red; uint8_t green; uint8_t blue;} rgb_entry_t;

static const rgb_entry_t rgb_palette[64] = {
  {0x80, 0x80, 0x80}, {0x00, 0x00, 0xBB}, {0x37, 0x00, 0xBF}, {0x84, 0x00, 0xA6}, {0xBB, 0x00, 0x6A}, {0xB7, 0x00, 0x1E},
  {0xB3, 0x00, 0x00}, {0x91, 0x26, 0x00}, {0x7B, 0x2B, 0x00}, {0x00, 0x3E, 0x00}, {0x00, 0x48, 0x0D}, {0x00, 0x3C, 0x22},
  {0x00, 0x2F, 0x66}, {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05}, {0x05, 0x05, 0x05}, {0xC8, 0xC8, 0xC8}, {0x00, 0x59, 0xFF},
  {0x44, 0x3C, 0xFF}, {0xB7, 0x33, 0xCC}, {0xFF, 0x33, 0xAA}, {0xFF, 0x37, 0x5E}, {0xFF, 0x37, 0x1A}, {0xD5, 0x4B, 0x00},
  {0xC4, 0x62, 0x00}, {0x3C, 0x7B, 0x00}, {0x1E, 0x84, 0x15}, {0x00, 0x95, 0x66}, {0x00, 0x84, 0xC4}, {0x11, 0x11, 0x11},
  {0x09, 0x09, 0x09}, {0x09, 0x09, 0x09}, {0xFF, 0xFF, 0xFF}, {0x00, 0x95, 0xFF}, {0x6F, 0x84, 0xFF}, {0xD5, 0x6F, 0xFF},
  {0xFF, 0x77, 0xCC}, {0xFF, 0x6F, 0x99}, {0xFF, 0x7B, 0x59}, {0xFF, 0x91, 0x5F}, {0xFF, 0xA2, 0x33}, {0xA6, 0xBF, 0x00},
  {0x51, 0xD9, 0x6A}, {0x4D, 0xD5, 0xAE}, {0x00, 0xD9, 0xFF}, {0x66, 0x66, 0x66}, {0x0D, 0x0D, 0x0D}, {0x0D, 0x0D, 0x0D},
  {0xFF, 0xFF, 0xFF}, {0x84, 0xBF, 0xFF}, {0xBB, 0xBB, 0xFF}, {0xD0, 0xBB, 0xFF}, {0xFF, 0xBF, 0xEA}, {0xFF, 0xBF, 0xCC},
  {0xFF, 0xC4, 0xB7}, {0xFF, 0xCC, 0xAE}, {0xFF, 0xD9, 0xA2}, {0xCC, 0xE1, 0x99}, {0xAE, 0xEE, 0xB7}, {0xAA, 0xF7, 0xEE},
  {0xB3, 0xEE, 0xFF}, {0xDD, 0xDD, 0xDD}, {0x11, 0x11, 0x11}, {0x11, 0x11, 0x11}
};

uint32_t sdl_palette[64];

void update_frame(void *reference, uint8_t* frame_data, int width, int height) {
  uint32_t *pixmem=screen->pixels;

  if(SDL_MUSTLOCK(screen)) {
    if(SDL_LockSurface(screen) < 0) {
      return;
    }
  }
  
  for(int p = 0; p < width*height; p++ )  {
    *pixmem=sdl_palette[*frame_data];
    pixmem++;
    frame_data++;
  }

  if(SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
  
  SDL_Flip(screen); 
}

uint16_t controller_read(void *reference) {
  return controller_data;
}

int main(int argc, char* argv[]) {
  nes_t nes_console;
  SDL_Event event;
  FILE *rom_file;
  uint8_t *rom_data;
  uint32_t rom_size;

  // load rom file
  if (argc<2) {
    printf("need rom file as argument\n");
    return 1;
  }

  rom_file=fopen(argv[1], "rb");

  if (rom_file==NULL) {
    printf("not able to open rom file %s\n", argv[1]);
    return 2;
  }

  fseek(rom_file, 0, SEEK_END);
  rom_size=ftell(rom_file);
  fseek(rom_file, 0, SEEK_SET);
  rom_data=malloc(rom_size);
  if (fread(rom_data, 1, rom_size, rom_file)!=rom_size) {
    printf("unable to read data from rom file %s\n", argv[1]);
    return 3;
  }
  fclose(rom_file);

  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    printf("unable to init sdl video\n");
    return 4;
  }
   
  if (!(screen = SDL_SetVideoMode(256, 240, 32, /*SDL_FULLSCREEN|*/SDL_HWSURFACE))) {
    SDL_Quit();
    printf("unable to set sdl video mode\n");
    return 5;
  }

  for(int idx=0; idx<64; idx++) {
    sdl_palette[idx]=SDL_MapRGB(screen->format, rgb_palette[idx].red, rgb_palette[idx].green, rgb_palette[idx].blue);
  }

  // init nes
  nes_init(&nes_console, 0, update_frame, controller_read);
  if (nes_setup_cartridge(&nes_console, rom_data, rom_size)!=0) {
    printf("unable to parse rom file\n");
    return 6;
  }
  
  int quit = 0;
  uint16_t key_value;
  
  nes_iterate_frame(&nes_console);

  while(!quit) {
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit=1;
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          key_value=0;
          switch (event.key.keysym.sym) {
            case SDLK_RIGHT: // controller Right
              key_value=0x80;
              break;
            case SDLK_LEFT: // controller Left
              key_value=0x40;
              break;
            case SDLK_DOWN: // controller Down
              key_value=0x20;
              break;
            case SDLK_UP: // controller Up
              key_value=0x10;
              break;
            case SDLK_q: // controller Start
              key_value=0x08;
              break;
            case SDLK_w: // controller Select
              key_value=0x04;
              break;
            case SDLK_s: // controller B
              key_value=0x02;
              break;
            case SDLK_a: // controller A
              key_value=0x01;
              break;
          }

          if (event.type==SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
              case SDLK_ESCAPE: 
                quit=1;
                break;
              case SDLK_n:
                nes_iterate_frame(&nes_console);
                break;
            }
            controller_data|=key_value;
          }
          else {
            controller_data&=~key_value;
          }
          break;
      }
    }
  }

  free(rom_data);
  SDL_Quit();
  
  return 0;
}

