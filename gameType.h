#ifndef __GAME_TYPE_H
#define __GAME_TYPE_H
#include "std.h"

typedef struct Vec2 {
    int x;
    int y;
} Vec2;

typedef struct RouteNode {
    Vec2 pos;
    struct RouteNode* next;
} RouteNode;

typedef struct GameConfig {
    int velocity; // velocity per second
    int drawMS; // refresh rate
    int readKeyForNDraw; // read key after n draw
    int timeoutForNDraw; // make movement after n draw
    int rHead;
    int rLine;
    int rWall;
    int height;
    int width;
    int beanAdd;
} GameConfig;

enum GameState {
    game_state_play,
    game_state_main_menu,
    game_state_pause,
    game_state_dead,
    game_state_exit,
    game_state_max,
};

typedef struct GameSnake {
    struct GameConfig conf;
    enum GameState state;
    int select;
    struct Vec2 direction;
    struct RouteNode *route;
    struct Vec2 moveTo;
    int framesLeft;
    int length;
    int lengthAdd;
    struct Vec2 bean;
} GameSnake;



#endif