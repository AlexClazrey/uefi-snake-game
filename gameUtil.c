#include "gameUtil.h"
#include "util.h"
#include <stdlib.h>
#include <time.h>

Vec2 makeVec2(int x, int y) {
    Vec2 res = {x, y};
    return res;
}

void gameInitConfig(GameSnake* snake) {
    snake->conf.velocity = 80;
    snake->conf.rLine = 10;
    snake->conf.rHead = 15;
    snake->conf.rWall = 40;
    getDisplayResolution(&snake->conf.width, &snake->conf.height);
    snake->conf.beanAdd = 30;
    snake->conf.drawMS = 20;
    snake->conf.readKeyForNDraw = 20;
    snake->conf.timeoutForNDraw = 10;
}

void gameInitRoute(GameSnake* snake) {
    RouteNode *head, *tail;
    snake->route = head = (RouteNode*)malloc(sizeof(RouteNode));
    head->pos = makeVec2(100, 200);
    head->next = tail = (RouteNode*)malloc(sizeof(RouteNode));
    tail->pos = makeVec2(100, 0);
    tail->next = NULL;
}

void gameAddRoute(GameSnake* snake) {
    RouteNode *newHead, *oldHead = snake->route;
    newHead = (RouteNode*)malloc(sizeof(RouteNode));
    newHead->pos = oldHead->pos;
    newHead->next = oldHead;
    snake->route = newHead;
}

void gameFreeRoute(GameSnake* snake) {
    RouteNode *start = snake->route;
    RouteNode *tmp;
    while(start) {
        tmp = start;
        start = start->next;
        free(tmp);
    }
}

// TODO not random, y = f(x) now.
void gameMakeBean(GameSnake* snake) {
    srand(time(0));
    int border = snake->conf.rWall + 50;
    int bx = random(border, snake->conf.width - border);
    int by = random(border, snake->conf.height - border);
    snake->bean.x = bx;
    snake->bean.y = by;
}
