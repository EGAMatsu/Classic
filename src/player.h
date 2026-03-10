// player.h — player base

#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "entity.h"

typedef struct Player {
    Entity e;
} Player;

void Player_init(Player* player, Level* level);

void Player_onTick(Player* player);
void Player_turn(Player* player, float dx, float dy);

#endif  // PLAYER_H