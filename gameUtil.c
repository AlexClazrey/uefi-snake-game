#include "gameUtil.h"
#include "util.h"
#include <Library/PrintLib.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define max(x1,x2) ((x1)<(x2)?(x2):(x1))
#define min(x1,x2) ((x1)<(x2)?(x1):(x2))

Vec2 makeVec2(int x, int y) {
    Vec2 res = {x, y};
    return res;
}

Vec2 vec2Mul(Vec2 vec, int x) {
    vec.x *= x;
    vec.y *= x;
    return vec;
}

Vec2 vec2MulD(Vec2 vec, double x) {
    vec.x = (int)(vec.x * x);
    vec.y = (int)(vec.y * x);
    return vec;
}

Vec2 vec2Plus(Vec2 a, Vec2 b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

Vec2 vec2Minus(Vec2 a, Vec2 b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

Vec2 vec2Swap(Vec2 a) {
    int t = a.x;
    a.x = a.y;
    a.y = t;
    return a;
}

int vec2Length(Vec2 a) {
    return (int)sqrt(a.x * a.x + a.y * a.y);
}

int vec2Dist(Vec2 a, Vec2 b) {
    return (int)sqrt((a.x - b.x) * (a.x - b.x)
        + (a.y - b.y) * (a.y - b.y));
}

int vec2RectDist(Vec2 a, Vec2 b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

void gameInitConfig(GameSnake* snake) {
    snake->conf.velocity = 140;
    snake->conf.rLine = 13;
    snake->conf.rHead = 15;
    snake->conf.rWall = 40;
    snake->conf.rBean = 7;
    getDisplayResolution(&snake->conf.width, &snake->conf.height);
    snake->conf.beanAdd = 50;
    snake->conf.beanVelocity = 10;
    snake->conf.cycleMS = 100;
    snake->conf.readKeyForNCycle = 1;
    snake->conf.timeoutForNCycle = 1;
    snake->conf.drawForNCycle = 1;
}

void gameInitRoute(GameSnake* snake) {
    RouteNode *head, *tail;
    snake->route = head = (RouteNode*)malloc(sizeof(RouteNode));
    head->pos = makeVec2(100, 200);
    head->next = tail = (RouteNode*)malloc(sizeof(RouteNode));
    tail->pos = makeVec2(100, 200);
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

void gameClearScreen() {
    pixel white = {255, 255, 255, 0};
    fillColor(white);
}

void gameDrawWall(GameSnake *snake) {
    pixel wall = {18, 7, 182, 0};
    int w = snake->conf.width - 1;
    int h = snake->conf.height - 1;
    int r = snake->conf.rWall;
    drawRect(r, 0, w, r, wall);
    drawRect(w-r, r, w, h, wall);
    drawRect(0, 0, r, h-r, wall);
    drawRect(0, h-r, w-r, h, wall);
}

void gameDrawBean(GameSnake *snake) {
    pixel bean = {179, 96, 191, 0};
    drawCircle(snake->bean.x, snake->bean.y, snake->conf.rBean, bean);
}

void gameDrawHead(GameSnake *snake) {
    pixel head = {133, 180, 26, 0};
    drawCircle(snake->route->pos.x, snake->route->pos.y, snake->conf.rHead, head);
}

static void gameDrawBodyPart(GameSnake* snake, Vec2 va, Vec2 vb) {
    pixel bodyColor = {149, 157, 85, 0};
    int rLine = snake->conf.rLine;
    int x1 = min(va.x, vb.x),
        x2 = max(va.x, vb.x),
        y1 = min(va.y, vb.y),
        y2 = max(va.y, vb.y);
    if(x1 == x2) {
        // draw vertical body part
        drawCircle(x1, y1, rLine, bodyColor);
        drawCircle(x1, y2, rLine, bodyColor);
        drawRect(x1-rLine, y1, x1+rLine, y2, bodyColor);
    } else if (y1 == y2) {
        // draw horizontal body part
        drawCircle(x1, y1, rLine, bodyColor);
        drawCircle(x2, y1, rLine, bodyColor);
        drawRect(x1, y1-rLine, x2, y2+rLine, bodyColor);
    } else {
        Print(L"Body Part Draw Error (%d, %d) -> (%d, %d)\n", va.x, va.y, vb.x, vb.y);
    }
}

static int gameCollisionBodyPart(GameSnake *snake, Vec2 va, Vec2 vb, int oldLengthRemain) {
    if(snake->length - oldLengthRemain < 30) {
        return 0; // not to collision with its neck
    }
    Vec2 head = snake->route->pos;
    // if vertical, rotate to horizontal
    if(va.x == vb.x) {
        va = vec2Swap(va);
        vb = vec2Swap(vb);
        head = vec2Swap(head);
    }
    if(va.y != vb.y) {
        // Print(L"Not supported (%d, %d) -> (%d, %d)\n", va.x, va.y, vb.x, vb.y);'
        return 1;
    }
    // swap to let va.x <= vb.x
    if(va.x > vb.x) {
        Vec2 tmp = va;
        va = vb;
        vb = tmp;
    }
    if(head.x >= va.x && head.x <= vb.x) {
        // body line collision
        int dist = abs(head.y - va.y);
        if(dist < snake->conf.rCollision) {
            return 1;
        } else {
            return 0;
        }
    } else {
        // body point collision
        Vec2 target = (head.x > vb.x) ? vb : va;
        if(vec2Dist(head, target) < snake->conf.rCollision) {
            return 1;
        } else {
            return 0;
        }
    }
}

int gameDrawBody(GameSnake* snake) {
    RouteNode* nodeA = snake->route;
    RouteNode* nodeB = nodeA->next;
    int lengthRemain = snake->length;
    int collision = 0;
    while(nodeB && lengthRemain > 0) {
        int dist = vec2Dist(nodeA->pos, nodeB->pos);
        Vec2 p2;
        int oldLengthRemain = lengthRemain;
        if(dist > lengthRemain) {
            Vec2 v2 = vec2MulD(vec2Minus(nodeB->pos, nodeA->pos), (double)lengthRemain/(double)dist);
            p2 = vec2Plus(nodeA->pos, v2);
            lengthRemain = 0;
        } else {
            p2 = nodeB->pos;
            lengthRemain -= dist;
        }
        gameDrawBodyPart(snake, nodeA->pos, p2);
        collision |= gameCollisionBodyPart(snake, nodeA->pos, p2, oldLengthRemain);
        nodeA = nodeB;
        nodeB = nodeB->next;
    }
    return collision;
}

void drawMenuOption(Vec2 pos, CHAR16* str, int selected) {
    if(selected) {
        pixel red = {0, 0, 255, 0};
        CHAR16 tmp = str[0];
        str[0] = L'>';
        drawString(pos.x, pos.y, str, 20, red);
        str[0] = tmp;
    } else {
        pixel black = {0, 0, 0, 0};
        drawString(pos.x, pos.y, str, 20, black);
    }
}

void gameDrawScore(GameSnake* snake) {
    static CHAR16 str[40];
    str[0]=L's';
    UnicodeSPrint(str, 40, L"Score: %d", snake->score);
    pixel white = {255, 255, 255, 0};
    drawString(350, 20, str, 10, white);
}
