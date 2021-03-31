#include <bcd.h>
#include <gb/gb.h>
#include <rand.h>
#include <stdbool.h>

#include "backgrounds/background.h"
#include "backgrounds/game_over.h"
#include "backgrounds/game_over_map.h"
#include "backgrounds/map.h"
#include "backgrounds/numerals.h"
#include "defines.h"
#include "metasprite.h"
#include "pipe.h"
#include "sprites/bird.h"
#include "sprites/button.h"
#include "sprites/pipe.h"

UINT8 g_background_position = 0;
UINT8 g_foreground_position = 0;
BCD g_player_score = 0;
BCD g_player_high_score = 0;
UINT8 g_foreground_scroll_speed = FOREGROUND_SCROLL_SPEED;
Metasprite g_bird = {
    .x = 30,
    .y = 70,
    .width = BIRD_WIDTH,
    .height = BIRD_HEIGHT,
    .velocity_x = 0,
    .velocity_y = 0,
    .sprite_ids = {0, 1, 2, 3},
    .tile_ids = {0, 1, 2, 3},
};

Metasprite g_button = {.x = 80,
                       .y = 68,
                       .width = 16,
                       .height = 16,
                       .velocity_x = 0,
                       .velocity_y = 0,
                       .sprite_ids = {4, 5, 6, 7},
                       .tile_ids = {BUTTON_TILE_START, BUTTON_TILE_START + 1,
                                    BUTTON_TILE_START + 2,
                                    BUTTON_TILE_START + 3}};

void on_hbl_interrupt() {
  switch (LYC_REG) {
  case 0x00:
    move_bkg(g_background_position, 0);
    LYC_REG = 0x78;
    break;
  case 0x78:
    move_bkg(g_foreground_position, 0);
    LYC_REG = 0x00;
    break;
  }
}

static inline void wait_frames(UINT8 frames) {
  static UINT8 wait_counter = 0;
  for (wait_counter = frames; wait_counter != 0; wait_counter--) {
    wait_vbl_done();
  }
}

inline void wait_for_button_press(UINT8 mask) {
  while (!(joypad() & mask)) {
    wait_vbl_done();
  }
}

static inline void display_score() {
  static UINT8 text_buffer[9];
  bcd2text(&g_player_score, NUMERALS_TILE_START, text_buffer);
  static UINT8 count;
  for (count = 0; count < 7; count++) {
    if (text_buffer[count] == NUMERALS_TILE_START) {
      text_buffer[count] = NUMERALS_TILE_START + 10;
    } else {
      break;
    }
  }
  move_win(WINDOW_CENTERING_OFFSET + (((7 - count) * TILE_SIZE) / 2), 133);
  set_win_tiles(0, 0, 8, 1, text_buffer);
}

static inline void update_bird_animation_state() {
  static UINT8 current_animation_frame = 0;
  switch (current_animation_frame) {
  case 0:
    g_bird.tile_ids[0] = 0;
    g_bird.tile_ids[1] = 1;
    current_animation_frame = 1;
    break;
  case 1:
    g_bird.tile_ids[0] = 4;
    g_bird.tile_ids[1] = 5;
    current_animation_frame = 2;
    break;
  case 2:
    g_bird.tile_ids[0] = 6;
    g_bird.tile_ids[1] = 7;
    current_animation_frame = 0;
    break;
  }
  metasprite_update_tiles(&g_bird);
}

static inline void update_button_animation_state() {
  static UINT8 current_animation_frame = 0;
  switch (current_animation_frame) {
  case 0:
    g_button.tile_ids[0] = BUTTON_TILE_START + 4;
    g_button.tile_ids[1] = BUTTON_TILE_START + 5;
    g_button.tile_ids[2] = BUTTON_TILE_START + 6;
    g_button.tile_ids[3] = BUTTON_TILE_START + 7;
    current_animation_frame = 1;
    break;
  case 1:
    g_button.tile_ids[0] = BUTTON_TILE_START;
    g_button.tile_ids[1] = BUTTON_TILE_START + 1;
    g_button.tile_ids[2] = BUTTON_TILE_START + 2;
    g_button.tile_ids[3] = BUTTON_TILE_START + 3;
    current_animation_frame = 0;
    break;
  }
  metasprite_update_tiles(&g_button);
}

inline void play_death_flash() {
  BGP_REG = 0x90;
  OBP0_REG = 0x90;
  wait_frames(3);
  BGP_REG = 0x40;
  OBP0_REG = 0x40;
  wait_frames(3);
  BGP_REG = 0x00;
  OBP0_REG = 0x00;
  wait_frames(3);
  BGP_REG = 0x40;
  OBP0_REG = 0x40;
  wait_frames(3);
  BGP_REG = 0x90;
  OBP0_REG = 0x90;
  wait_frames(3);
  BGP_REG = 0xE4;
  OBP0_REG = 0xE4;
}

inline void play_jump_sound() {
  NR10_REG = 0x16;
  NR11_REG = 0x40;
  NR12_REG = 0x73;
  NR13_REG = 0x00;
  NR14_REG = 0xC3;
}

void main(void) {
  // Init sound
  NR52_REG = 0x80;
  NR50_REG = 0x77;
  NR51_REG = 0xFF;
  // Load Background
  set_bkg_data(0, 10, BG_TILE_DATA);
  set_bkg_data(NUMERALS_TILE_START, NUMERALS_TILE_START + 10,
               NUMERALS_TILE_DATA);
  set_bkg_data(GAME_OVER_TILE_START, GAME_OVER_TILE_START + 10,
               GAME_OVER_TILE_DATA);
  set_bkg_tiles(0, 0, BG_TILE_MAPWidth, BG_TILE_MAPHeight, BG_TILE_MAP);
  // Load tiles into VRAM
  set_sprite_data(0, 8, BIRD_TILE_DATA);
  set_sprite_data(PIPE_TILE_START, PIPE_TILE_START + 4, PIPE_TILE_DATA);
  set_sprite_data(BUTTON_TILE_START, BUTTON_TILE_START + 7, BUTTON_TILE_DATA);

  STAT_REG = 0x45; // Enable H-Blank Interrupt
  LYC_REG = 0x00;

  disable_interrupts();
  add_LCD(on_hbl_interrupt);
  enable_interrupts();

  set_interrupts(VBL_IFLAG | LCD_IFLAG);

  metasprite_init(&g_bird);

  SHOW_BKG;
  SHOW_SPRITES;
  SHOW_WIN;
  DISPLAY_ON;

  while (true) {

    metasprite_move_to(&g_bird, 30, 70);
    set_win_tiles(0, 0, 20, 2, NUMERALS_TILE_MAP);
    g_player_score = 0;
    display_score();
    metasprite_init(&g_button);

    static UINT8 last_button_state;
    static UINT8 button_state;
    static UINT8 frame_count = 0;
    while (true) {
      button_state = joypad();
      if (button_state & J_A) {
        play_jump_sound();
        metasprite_jump(&g_bird);
        last_button_state = button_state;
        break;
      }
      if (frame_count % CYCLE_ANIMATION_N_FRAMES == 0) {
        update_bird_animation_state();
      }
      if (frame_count % CYCLE_BUTTON_N_FRAMES == 0) {
        update_button_animation_state();
      }
      frame_count += 1;
      g_background_position += BACKGROUND_SCROLL_SPEED;
      g_foreground_position += g_foreground_scroll_speed;
      wait_vbl_done();
    }

    initrand(DIV_REG);

    static Pipe pipe;
    pipe.x = 200;
    pipe.gap_skip = 8;
    for (UINT8 i = 0; i < 36; i++) {
      pipe.sprite_ids[i] = i + 4;
    }
    pipe_init(&pipe);
    pipe_move_gap_height(&pipe, 2 + (((UINT8)rand()) % (15 - pipe.gap_skip)));

    while (true) {
      pipe_scroll_horizontal(&pipe, -1 * g_foreground_scroll_speed);
      if (pipe.x == 0xF0) {
        static const BCD score_per_pipe = 1;
        static BCD *const spp_ref = &score_per_pipe;

        bcd_add(&g_player_score, spp_ref);
        display_score();
        switch (g_player_score) {
        case 0x20:
          pipe.gap_skip = 7;
          break;
        case 0x35:
          pipe.gap_skip = 6;
          break;
        case 0x50:
          g_foreground_scroll_speed = 3;
          break;
        }
        pipe_move_gap_height(&pipe,
                             2 + (((UINT8)rand()) % (15 - pipe.gap_skip)));
        pipe_scroll_horizontal(&pipe, -78);
      }
      button_state = joypad();
      if ((button_state & J_A) && !(last_button_state & J_A)) {
        play_jump_sound();
        metasprite_jump(&g_bird);
      }
      if (frame_count % APPLY_GRAVITY_N_FRAMES == 0) {
        metasprite_apply_gravity(&g_bird);
      }
      if (frame_count % CYCLE_ANIMATION_N_FRAMES == 0) {
        update_bird_animation_state();
      }
      frame_count += 1;
      g_background_position += BACKGROUND_SCROLL_SPEED;
      g_foreground_position += g_foreground_scroll_speed;
      metasprite_update_position_using_velocity(&g_bird);
      if (g_bird.y < 0x05) {
        g_bird.y = 0x05;
        g_bird.velocity_y = 0x00;
      }
      if (g_bird.y + g_bird.height > 0x85) {
        break;
      }
      if (g_bird.x > (pipe.x - g_bird.width) &&
          g_bird.x < pipe.x + PIPE_WIDTH) {
        if (g_bird.y < ((pipe.height_of_gap + 1) * TILE_SIZE) ||
            g_bird.y + g_bird.height > ((pipe.height_of_gap * TILE_SIZE) +
                                        (pipe.gap_skip * TILE_SIZE))) {
          break;
        }
      }
      last_button_state = button_state;
      wait_vbl_done();
    }
    play_death_flash();
    pipe_hide(&pipe);
    metasprite_hide(&g_bird);
    set_sprite_prop(g_button.sprite_ids[0], 0);
    set_sprite_prop(g_button.sprite_ids[1], 0);
    set_sprite_prop(g_button.sprite_ids[2], 0);
    set_sprite_prop(g_button.sprite_ids[3], 0);
    g_foreground_scroll_speed = FOREGROUND_SCROLL_SPEED;
    if (g_player_score > g_player_high_score) {
      g_player_high_score = g_player_score;
    }
    static UINT8 score_buffer[9];
    static UINT8 high_score_buffer[9];
    bcd2text(&g_player_score, NUMERALS_TILE_START, score_buffer);
    bcd2text(&g_player_high_score, NUMERALS_TILE_START, high_score_buffer);
    BGP_REG = 0xC0;
    set_win_tiles(0, 0, 20, 2, NUMERALS_TILE_CLEAR);
    set_win_tiles(6, 4, GAME_OVER_MAPWidth, GAME_OVER_MAPHeight, GAME_OVER_MAP);
    set_win_tiles(6, 8, 8, 1, score_buffer);
    set_win_tiles(6, 12, 8, 1, high_score_buffer);
    move_win(0, 0);
    wait_frames(180);
    BGP_REG = 0xE4;
  }
}
