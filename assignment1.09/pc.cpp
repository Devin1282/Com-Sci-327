#include <stdlib.h>
#include <ncurses.h>
#include <string>

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "object.h"

int get_empty_inventory_slot(dungeon_t *d)
{
  int i;

  for (i = 0; i < 10; i++) {
    if (d->thepc->inventory[i] == 0) {
      return i;
    }
  }
  return 15;
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->thepc->alive;
}

void place_pc(dungeon_t *d)
{
  d->thepc->position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->thepc->position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));

  pc_init_known_terrain(d->thepc);
  pc_observe_terrain(d->thepc, d);
}

void pc_config(dungeon_t *d)
{
  static dice pc_dice(0, 1, 4);
  int i;

  d->thepc = new pc;

  d->thepc->symbol = '@';

  place_pc(d);

  d->thepc->speed = PC_SPEED;
  d->thepc->speed = 50;
  d->thepc->alive = 1;
  d->thepc->sequence_number = 0;
  d->thepc->color.push_back(COLOR_WHITE);
  d->thepc->damage = &pc_dice;
  d->thepc->name = "Isabella Garcia-Shapiro";
  d->thepc->hp = 1000;

  for (i = 0; i < 10; i++) {
    d->thepc->inventory[i] = 0;
  }

  for (i = 0; i < 12; i++) {
    d->thepc->equipment[i] = 0;
  }

  d->charmap[d->thepc->position[dim_y]][d->thepc->position[dim_x]] = d->thepc;

  dijkstra(d);
  dijkstra_tunnel(d);
}

/* Not used anymore, but leaving it here because you could *
 * adapt it if you wanted to implement a bot player.       */
uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  dir[dim_y] = dir[dim_x] = 0;

  /* Tunnel to the nearest dungeon corner, then move around in hopes *
   * of killing a couple of monsters before we die ourself.          */

  if (in_corner(d, d->thepc)) {
    /*
    dir[dim_x] = (mapxy(d->thepc->position[dim_x] - 1,
                        d->thepc->position[dim_y]) ==
                  ter_wall_immutable) ? 1 : -1;
    */
    dir[dim_y] = (mapxy(d->thepc->position[dim_x],
                        d->thepc->position[dim_y] - 1) ==
                  ter_wall_immutable) ? 1 : -1;
  } else {
    dir_nearest_wall(d, d->thepc, dir);
  }

  return 0;
}

void pc_learn_terrain(pc *p, pair_t pos, terrain_type_t ter)
{
  p->known_terrain[pos[dim_y]][pos[dim_x]] = ter;
  p->visible[pos[dim_y]][pos[dim_x]] = 1;
}

void pc_reset_visibility(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->visible[y][x] = 0;
    }
  }
}

terrain_type_t pc_learned_terrain(pc *p, int8_t y, int8_t x)
{
  return p->known_terrain[y][x];
}

void pc_init_known_terrain(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->known_terrain[y][x] = ter_unknown;
      p->visible[y][x] = 0;
    }
  }
}

void pc_observe_terrain(pc *p, dungeon_t *d)
{
  pair_t where;
  int8_t y_min, y_max, x_min, x_max;

  y_min = p->position[dim_y] - PC_VISUAL_RANGE;
  if (y_min < 0) {
    y_min = 0;
  }
  y_max = p->position[dim_y] + PC_VISUAL_RANGE;
  if (y_max > DUNGEON_Y - 1) {
    y_max = DUNGEON_Y - 1;
  }
  x_min = p->position[dim_x] - PC_VISUAL_RANGE;
  if (x_min < 0) {
    x_min = 0;
  }
  x_max = p->position[dim_x] + PC_VISUAL_RANGE;
  if (x_max > DUNGEON_X - 1) {
    x_max = DUNGEON_X - 1;
  }

  for (where[dim_y] = y_min; where[dim_y] <= y_max; where[dim_y]++) {
    where[dim_x] = x_min;
    can_see(d, p->position, where, 1);
    where[dim_x] = x_max;
    can_see(d, p->position, where, 1);
  }
  /* Take one off the x range because we alreay hit the corners above. */
  for (where[dim_x] = x_min - 1; where[dim_x] <= x_max - 1; where[dim_x]++) {
    where[dim_y] = y_min;
    can_see(d, p->position, where, 1);
    where[dim_y] = y_max;
    can_see(d, p->position, where, 1);
  }       
}

int32_t is_illuminated(pc *p, int8_t y, int8_t x)
{
  return p->visible[y][x];
}

void pc_see_object(character *the_pc, object *o)
{
  if (o) {
    o->has_been_seen();
  }
}
