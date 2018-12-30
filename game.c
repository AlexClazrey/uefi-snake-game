#include "game.h"
#include "util.h"
#include "gameUtil.h"
#include <Library/PrintLib.h>

int pauseMenuCount = 2;
CHAR16* pauseMenuText[2];
int mainMenuCount = 2;
CHAR16* mainMenuText[2];

static void gameInit(GameSnake* snake) {
    pauseMenuText[0] = L" 1. Continue";
    pauseMenuText[1] = L" 2. Return to main menu";
    mainMenuText[0] = L" 1. Start";
    mainMenuText[1] = L" 2. Exit";
    gameInitConfig(snake);
    snake->state = game_state_main_menu;
    snake->select = 0;
    snake->direction = makeVec2(1, 0);
    gameInitRoute(snake);
    snake->moveTo = makeVec2(0, 0);
    snake->framesLeft = 0;
    snake->length = 60;
    snake->lengthAdd = 0;
    snake->score = 0;
    gameMakeBean(snake);
}

static void gameFree(GameSnake* snake) {
    gameFreeRoute(snake);
}

static et gameKey(GameSnake* snake) {
    et status;
    UINT16 scanCode;
    CHAR16 unicode;
    status = utilReadLastKey(&scanCode, &unicode);
    if(snake->state == game_state_play) {
        if(scanCode == SCAN_ESC) {
            snake->state = game_state_pause;
        } else if (scanCode == SCAN_UP) {
            if(snake->direction.y != 1)
                snake->direction = makeVec2(0, -1);
        } else if (scanCode == SCAN_DOWN) {
            if(snake->direction.y != -1)
                snake->direction = makeVec2(0, 1);
        } else if (scanCode == SCAN_LEFT) {
            if(snake->direction.x != 1)
                snake->direction = makeVec2(-1, 0);
        } else if (scanCode == SCAN_RIGHT) {
            if(snake->direction.x != 1)
                snake->direction = makeVec2(1, 0);
        }
        if(scanCode == SCAN_UP || scanCode == SCAN_DOWN || scanCode == SCAN_LEFT || scanCode == SCAN_RIGHT) {
            gameAddRoute(snake);
        }
    } else {
        if(scanCode == SCAN_UP) {
            if(snake->select > 0) 
                snake->select--;
        } else if (scanCode == SCAN_DOWN) {
            if(snake->select < 1)
                snake->select++;
        } else if (unicode == CHAR_CARRIAGE_RETURN) {
            // change to different menu
            if(snake->state == game_state_main_menu) {
                if(snake->select == 0) {
                    snake->state = game_state_play;
                } else {
                    snake->state = game_state_exit;
                }
            } else if(snake->state == game_state_pause) {
                if(snake->select == 0) {
                    snake->state = game_state_play;
                } else {
                    gameInit(snake);
                }
            } else if(snake->state == game_state_dead) {
                // back to main menu
                gameInit(snake);
            }
            // clear select
            snake->select = 0;
        }
    }
    return status;
}

static et gameTime(GameSnake* snake) {
    et status = EFI_SUCCESS;
    if(snake->state == game_state_play) {
        // set move target
        int distance = snake->conf.velocity * snake->conf.cycleMS * snake->conf.timeoutForNCycle / 1000;
        // dynamic collision settings
        snake->conf.rCollision = distance;
        snake->conf.rHead = distance + 1;
        snake->conf.rLine = distance - 1;
        // moveTo
        snake->moveTo = vec2Plus(snake->route->pos, vec2Mul(snake->direction, distance));
        snake->framesLeft = snake->conf.timeoutForNCycle;
        // check collision
        Vec2 headPos = snake->moveTo;
        // wall collision
        int rAll = snake->conf.rWall + snake->conf.rHead;
        if(headPos.x < rAll || headPos.x > snake->conf.width - rAll
        || headPos.y < rAll || headPos.y > snake->conf.height - rAll) {
            snake->state = game_state_dead;
        }
        // bean collision
        if(vec2RectDist(headPos, snake->bean) < snake->conf.rHead + snake->conf.rBean) {
            snake->lengthAdd = snake->conf.beanAdd;
            snake->conf.velocity += snake->conf.beanVelocity;
            snake->score++;
            gameMakeBean(snake);
        }
    }
    return status;
}

#define min(x1,x2) ((x1)<(x2)?(x1):(x2))

static et gameDraw(GameSnake* snake) {
    et status = EFI_SUCCESS;
    // clear screen
    gameClearScreen();
    // check mode
    if(snake->state == game_state_play) {
        // draw wall
        gameDrawWall(snake);
        // draw bean
        gameDrawBean(snake);
        // update move progress
        if(snake->framesLeft > 0) {
            Vec2 v2 = vec2MulD(vec2Minus(snake->moveTo, snake->route->pos), 1.0/snake->framesLeft);
            snake->route->pos = vec2Plus(snake->route->pos, v2);
            // snake grows
            int v2len = vec2Length(v2);
            int dl = min(v2len, snake->lengthAdd);
            snake->length += dl;
            snake->lengthAdd -= dl;
            // 
            snake->framesLeft--;
        }
        // draw body and collision check
        int collision = gameDrawBody(snake);
        // draw head
        gameDrawHead(snake);
        if(collision) {
            snake->state = game_state_dead;
        }
        // draw score
        gameDrawScore(snake);
    } else if(snake->state == game_state_main_menu) {
        // draw main menu
        for(int i = 0; i < mainMenuCount; i++) {
            drawMenuOption(makeVec2(200, 100 + 40 * i), mainMenuText[i], snake->select == i);
        }
    } else if(snake->state == game_state_pause) {
        // draw pause menu
        for(int i = 0; i < pauseMenuCount; i++) {
            drawMenuOption(makeVec2(440, 350 + 40 * i), pauseMenuText[i], snake->select == i);
        }
    } else if(snake->state == game_state_dead) {
        // draw body
        gameDrawBody(snake);
        // draw head
        gameDrawHead(snake);
        // draw dead scene
        pixel black = {0, 0, 0, 0};
        drawString(350, 400, L"Game Over", 20, black);
        CHAR16 buf[40];
        UnicodeSPrint(buf, 40, L"Your score: %d", snake->score);
        drawString(330, 430, buf, 10, black);
        drawMenuOption(makeVec2(300, 500), L" Return To Main Menu", 1);
    }
    return status;
}

et gameCycle() {
    et status = EFI_SUCCESS;
    GameSnake snake;
    initDisplay();
    gameInit(&snake);
    int keyCycleCount = 0, timeCycleCount = 0, drawCycleCount = 0;
    while(1) {
        // key process
        if(++keyCycleCount >= snake.conf.readKeyForNCycle) {
            gameKey(&snake);
            keyCycleCount = 0;
        }
        // time process
        if(++timeCycleCount >= snake.conf.timeoutForNCycle) {
            gameTime(&snake);
            timeCycleCount = 0;
        }
        // draw process
        if(++drawCycleCount >= snake.conf.drawForNCycle) {
            gameDraw(&snake);
            drawCycleCount = 0;
        }
        if(snake.state == game_state_exit) {
            break;
        }
        utilSleep(snake.conf.cycleMS);
    }
    freeDisplay();
    gameFree(&snake);
    return status;
}
