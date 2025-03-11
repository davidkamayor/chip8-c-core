#include <stdbool.h>
#include <stdio.h>

#define RAM_SIZE 4096
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE SCREEN_WIDTH *SCREEN_HEIGHT
#define STACK_SIZE 16
#define NUM_KEYS 16
#define FONTSET_SIZE 80

struct Emu {
  u_int16_t program_counter;
  u_int8_t ram[RAM_SIZE];
  bool screen[SCREEN_SIZE];
  u_int8_t v_regs[16];
  u_int16_t i_reg;
  u_int16_t stack_pointer;
  u_int16_t stack[STACK_SIZE];
  u_int8_t delay_timer;
  u_int8_t sound_timer;
  u_int16_t opcode;
};

void initialize_emu(struct Emu *emu) {
  emu->program_counter =
      0x200; // Program counter starts at 0x200 (standard for CHIP-8)
  emu->i_reg = 0;
  emu->stack_pointer = 0;
  emu->delay_timer = 0;
  emu->sound_timer = 0;
  emu->opcode = 0000;

  u_int8_t FONTSET[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // move Fonts to ram
  for (int i = 0; i < FONTSET_SIZE; i++) {
    emu->ram[i] = FONTSET[i];
  }

  // Initialize RAM (set to 0)
  for (int i = FONTSET_SIZE; i < RAM_SIZE; i++) {
    emu->ram[i] = 0;
  }

  // Initialize the screen (set all pixels to false, meaning off)
  for (int i = 0; i < SCREEN_SIZE; i++) {
    emu->screen[i] = false;
  }

  // Initialize V registers to 0
  for (int i = 0; i < 16; i++) {
    emu->v_regs[i] = 0;
  }

  // Initialize stack to 0
  for (int i = 0; i < STACK_SIZE; i++) {
    emu->stack[i] = 0;
  }
}

int execute(struct Emu *emu) {
  switch (emu->opcode) {
  case 0x0000: // Nop
    return 0;
  case 0x00E0: // Clear Screen
    for (int i = 0; i < SCREEN_SIZE; i++) {
      emu->screen[i] = false;
    }
  case 0x00EE: // Return from Subroutine
    u_int16_t return_addr = pop(&emu);
    emu->stack_pointer = return_addr;
  }
  return 0;
}

void tick(struct Emu *emu) {
  // inline of "fetch" function
  u_int8_t higher_byte = emu->ram[emu->program_counter];
  u_int8_t lower_byte = emu->ram[emu->program_counter + 1];
  u_int16_t opcode = (higher_byte << 8) | lower_byte;
  emu->program_counter += 2;
  execute(opcode);
}

int push(struct Emu *emu, u_int16_t val) {
  emu->stack[emu->stack_pointer] = val;
  emu->stack_pointer++;
  return 0;
}

int pop(struct Emu *emu) {
  emu->stack_pointer--;
  return emu->stack[emu->stack_pointer];
}

int mem_dump(struct Emu *emu) {
  for (int i = 0; i < RAM_SIZE; i++) {
    printf("%d", emu->ram[i]);
  }
  return 0;
}

int main() {
  struct Emu emu;
  initialize_emu(&emu);
  return 0;
}