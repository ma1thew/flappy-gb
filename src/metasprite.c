#include "metasprite.h"

inline void metasprite_init(Metasprite *metasprite) {
  metasprite_update_tiles(metasprite);
  metasprite_move_to(metasprite, metasprite->x, metasprite->y);
}

inline void metasprite_move_offset(Metasprite *metasprite,
                                   ScreenCoordinate offset_x,
                                   ScreenCoordinate offset_y) {
  metasprite_move_to(metasprite, metasprite->x + offset_x,
                     metasprite->y + offset_y);
}

// TODO: consider height and width
inline void metasprite_move_to(Metasprite *metasprite, ScreenCoordinate new_x,
                               ScreenCoordinate new_y) {
  metasprite->x = new_x;
  metasprite->y = new_y;
  move_sprite(metasprite->sprite_ids[0], new_x, new_y);
  move_sprite(metasprite->sprite_ids[1], new_x, new_y + TILE_SIZE);
  move_sprite(metasprite->sprite_ids[2], new_x + TILE_SIZE, new_y);
  move_sprite(metasprite->sprite_ids[3], new_x + TILE_SIZE, new_y + TILE_SIZE);
}

inline void metasprite_apply_gravity(Metasprite *metasprite) {
  metasprite->velocity_y += GRAVITY_ACCELERATION;
}

inline void metasprite_update_position_using_velocity(Metasprite *metasprite) {
  metasprite->x += metasprite->velocity_x;
  metasprite->y += metasprite->velocity_y;
  metasprite_move_to(metasprite, metasprite->x, metasprite->y);
}

inline void metasprite_jump(Metasprite *metasprite) {
  metasprite->velocity_y = JUMP_VELOCITY;
}

inline void metasprite_update_tiles(Metasprite *metasprite) {
  set_sprite_tile(metasprite->sprite_ids[0], metasprite->tile_ids[0]);
  set_sprite_tile(metasprite->sprite_ids[1], metasprite->tile_ids[1]);
  set_sprite_tile(metasprite->sprite_ids[2], metasprite->tile_ids[2]);
  set_sprite_tile(metasprite->sprite_ids[3], metasprite->tile_ids[3]);
}

inline void metasprite_hide(Metasprite *metasprite) {
  move_sprite(metasprite->sprite_ids[0], 0, 0);
  move_sprite(metasprite->sprite_ids[1], 0, 0);
  move_sprite(metasprite->sprite_ids[2], 0, 0);
  move_sprite(metasprite->sprite_ids[3], 0, 0);
}
