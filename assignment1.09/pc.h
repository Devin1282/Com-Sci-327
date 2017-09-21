#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
# include "character.h"
# include "dungeon.h"

class pc : public character {
 public:
  terrain_type_t known_terrain[DUNGEON_Y][DUNGEON_X];
  unsigned char visible[DUNGEON_Y][DUNGEON_X];
  object *inventory[10];
  object *equipment[12];
};

class object;
typedef struct dungeon dungeon_t;
#include "dungeon.h"

int get_empty_inventory_slot(dungeon_t *d);
uint32_t pc_is_alive(dungeon_t *d);
void pc_config(dungeon_t *d);
uint32_t pc_next_pos(dungeon_t *d, pair_t dir);
void place_pc(dungeon_t *d);
void pc_learn_terrain(pc *p, pair_t pos, terrain_type_t ter);
terrain_type_t pc_learned_terrain(pc *p, int8_t y, int8_t x);
void pc_init_known_terrain(pc *p);
void pc_observe_terrain(pc *p, dungeon_t *d);
int32_t is_illuminated(pc *p, int8_t y, int8_t x);
void pc_reset_visibility(pc *p);
void pc_see_object(character *the_pc, object *o);

#endif
