#pragma once
#include <gb/gb.h>

#include "defines.h"

typedef struct Metasprite {
  ScreenCoordinate x;
  ScreenCoordinate y;
  UINT8 width;
  UINT8 height;
  INT8 velocity_x;
  INT8 velocity_y;
  UINT8 sprite_ids[4]; // A metasprite is four sprites.
  UINT8 tile_ids[4];   // A metasprite is four sprites.
} Metasprite;

inline void metasprite_init(Metasprite *metasprite);
inline void metasprite_move_to(Metasprite *metasprite, ScreenCoordinate new_x,
                               ScreenCoordinate new_y);
inline void metasprite_move_offset(Metasprite *metasprite,
                                   ScreenCoordinate offset_x,
                                   ScreenCoordinate offset_y);
inline void metasprite_apply_gravity(Metasprite *metasprite);
inline void metasprite_update_position_using_velocity(Metasprite *metasprite);
inline void metasprite_jump(Metasprite *metasprite);
inline void metasprite_update_tiles(Metasprite *metasprite);
inline void metasprite_hide(Metasprite *metasprite);
