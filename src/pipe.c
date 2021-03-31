#include "pipe.h"
#include "defines.h"

inline void pipe_init(Pipe *pipe) {
  set_sprite_tile(pipe->sprite_ids[PIPE_TILE_TOP_LEFT],
                  PIPE_TILE_START + CAP_LEFT);
  set_sprite_prop(pipe->sprite_ids[PIPE_TILE_TOP_LEFT], S_FLIPY);

  set_sprite_tile(pipe->sprite_ids[PIPE_TILE_TOP_MIDDLE],
                  PIPE_TILE_START + CAP_MIDDLE);
  set_sprite_prop(pipe->sprite_ids[PIPE_TILE_TOP_MIDDLE], S_FLIPY);

  set_sprite_tile(pipe->sprite_ids[PIPE_TILE_TOP_RIGHT],
                  PIPE_TILE_START + CAP_LEFT);
  set_sprite_prop(pipe->sprite_ids[PIPE_TILE_TOP_RIGHT], S_FLIPX | S_FLIPY);

  set_sprite_tile(pipe->sprite_ids[PIPE_TILE_BOTTOM_LEFT],
                  PIPE_TILE_START + CAP_LEFT);

  set_sprite_tile(pipe->sprite_ids[PIPE_TILE_BOTTOM_MIDDLE],
                  PIPE_TILE_START + CAP_MIDDLE);

  set_sprite_tile(pipe->sprite_ids[PIPE_TILE_BOTTOM_RIGHT],
                  PIPE_TILE_START + CAP_LEFT);
  set_sprite_prop(pipe->sprite_ids[PIPE_TILE_BOTTOM_RIGHT], S_FLIPX);

  for (UINT8 i = 6; i < 16; i++) {
    set_sprite_tile(pipe->sprite_ids[i], PIPE_TILE_START + MAIN_LEFT);
  }
  for (UINT8 i = 16; i < 26; i++) {
    set_sprite_tile(pipe->sprite_ids[i], PIPE_TILE_START + MAIN_MIDDLE);
  }
  for (UINT8 i = 26; i < 36; i++) {
    set_sprite_tile(pipe->sprite_ids[i], PIPE_TILE_START + MAIN_LEFT);
    set_sprite_prop(pipe->sprite_ids[i], S_FLIPX);
  }
}

inline void pipe_scroll_horizontal(Pipe *pipe, ScreenCoordinate x) {
  pipe->x += x;
  for (UINT8 i = 36; i != 0; i--) {
    scroll_sprite(pipe->sprite_ids[i - 1], x, 0);
  }
}

inline void pipe_move_gap_height(Pipe *pipe, UINT8 gap_height) {
  pipe->height_of_gap = gap_height;
  for (UINT8 i = 0; i < gap_height; i++) {
    move_sprite(pipe->sprite_ids[i + 4], pipe->x, i * TILE_SIZE);
    move_sprite(pipe->sprite_ids[i + 14], pipe->x + TILE_SIZE, i * TILE_SIZE);
    move_sprite(pipe->sprite_ids[i + 24], pipe->x + TILE_SIZE + TILE_SIZE,
                i * TILE_SIZE);
  }
  move_sprite(pipe->sprite_ids[PIPE_TILE_TOP_LEFT], pipe->x,
              gap_height * TILE_SIZE);
  move_sprite(pipe->sprite_ids[PIPE_TILE_TOP_MIDDLE], pipe->x + TILE_SIZE,
              gap_height * TILE_SIZE);
  move_sprite(pipe->sprite_ids[PIPE_TILE_TOP_RIGHT],
              pipe->x + TILE_SIZE + TILE_SIZE, gap_height * TILE_SIZE);

  move_sprite(pipe->sprite_ids[PIPE_TILE_BOTTOM_LEFT], pipe->x,
              (gap_height + pipe->gap_skip) * TILE_SIZE);
  move_sprite(pipe->sprite_ids[PIPE_TILE_BOTTOM_MIDDLE], pipe->x + TILE_SIZE,
              (gap_height + pipe->gap_skip) * TILE_SIZE);
  move_sprite(pipe->sprite_ids[PIPE_TILE_BOTTOM_RIGHT],
              pipe->x + TILE_SIZE + TILE_SIZE,
              (gap_height + pipe->gap_skip) * TILE_SIZE);

  for (UINT8 i = gap_height; i < (16 - pipe->gap_skip); i++) {
    move_sprite(pipe->sprite_ids[i + 4], pipe->x,
                (i + pipe->gap_skip + 1) * TILE_SIZE);
    move_sprite(pipe->sprite_ids[i + 14], pipe->x + TILE_SIZE,
                (i + pipe->gap_skip + 1) * TILE_SIZE);
    move_sprite(pipe->sprite_ids[i + 24], pipe->x + TILE_SIZE + TILE_SIZE,
                (i + pipe->gap_skip + 1) * TILE_SIZE);
  }
}

inline void pipe_hide(Pipe *pipe) {
  pipe->x += 0;
  for (UINT8 i = 36; i != 0; i--) {
    move_sprite(pipe->sprite_ids[i - 1], 0, 0);
  }
}
