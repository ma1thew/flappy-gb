#pragma once
#include <gb/gb.h>
#include <stdbool.h>

#include "defines.h"

enum SPECIAL_SPRITES {
  PIPE_TILE_TOP_LEFT,
  PIPE_TILE_TOP_MIDDLE,
  PIPE_TILE_TOP_RIGHT,
  PIPE_TILE_BOTTOM_LEFT,
  PIPE_TILE_BOTTOM_MIDDLE,
  PIPE_TILE_BOTTOM_RIGHT,
};

enum PIPE_TILES {
  CAP_LEFT,
  CAP_MIDDLE,
  MAIN_LEFT,
  MAIN_MIDDLE,
};

typedef struct Pipe {
  UINT8 x;
  UINT8 height_of_gap;
  UINT8 gap_skip;
  UINT8 sprite_ids[36]; // 15 high, minus 3 tile gap, times 3 wide = 36
} Pipe;

inline void pipe_init(Pipe *pipe);
inline void pipe_scroll_horizontal(Pipe *pipe, ScreenCoordinate x);
inline void pipe_move_gap_height(Pipe *pipe, UINT8 gap_height);
inline void pipe_hide(Pipe *pipe);
