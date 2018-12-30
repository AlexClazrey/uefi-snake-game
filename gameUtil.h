#ifndef __GAME_UTIL_H_
#define __GAME_UTIL_H_
#include "std.h"
#include "gameType.h"

Vec2 makeVec2(int x, int y);
Vec2 vec2Mul(Vec2 v, int x);
Vec2 vec2MulD(Vec2 v, double x);
Vec2 vec2Plus(Vec2 a, Vec2 b);
Vec2 vec2Minus(Vec2 a, Vec2 b);
int vec2Length(Vec2 a);
int vec2Dist(Vec2 a, Vec2 b);
int vec2RectDist(Vec2 a, Vec2 b);
void gameInitConfig(GameSnake* snake);
void gameInitRoute(GameSnake* snake);
void gameAddRoute(GameSnake* snake);
void gameFreeRoute(GameSnake* snake);
void gameMakeBean(GameSnake* snake);
void gameClearScreen();
void gameDrawWall(GameSnake *snake);
void gameDrawBean(GameSnake *snake);
void gameDrawHead(GameSnake *snake);
int gameDrawBody(GameSnake* snake);
void drawMenuOption(Vec2 pos, CHAR16* str, int selected);
void gameDrawScore(GameSnake* snake);

#endif