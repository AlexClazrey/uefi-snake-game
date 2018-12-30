#ifndef __GAME_UTIL_H_
#define __GAME_UTIL_H_
#include "std.h"
#include "gameType.h"

Vec2 makeVec2(int x, int y);
void gameInitConfig(GameSnake* snake);
void gameInitRoute(GameSnake* snake);
void gameAddRoute(GameSnake* snake);
void gameFreeRoute(GameSnake* snake);
void gameMakeBean(GameSnake* snake);

#endif