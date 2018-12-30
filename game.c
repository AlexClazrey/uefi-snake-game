#include "game.h"
#include "util.h"
#include "gameUtil.h"

static void gameInit(GameSnake* snake) {
    gameInitConfig(snake);
    snake->state = game_state_main_menu;
    snake->select = 0;
    snake->direction = makeVec2(1, 0);
    gameInitRoute(snake);
    snake->moveTo = makeVec2(0, 0);
    snake->framesLeft = 0;
    snake->length = 60;
    snake->lengthAdd = 0;
    gameMakeBean(snake);
}

static et gameKey(GameSnake* snake) {
    et status;
    UINT16 scanCode;
    CHAR16 unicode;

    return status;
}

static et gameTime(GameSnake* snake) {
    et status;
    return status;
}

static et gameDraw(GameSnake* snake) {
    et status;
    return status;
}

et gameCycle() {
    et status;
    GameSnake snake;
    initDisplay();
    gameInit(&snake);
    int keyCycleCount = 0, timeCycleCount = 0;
    while(1) {
        // key process
        if(keyCycleCount++ >= snake.conf.readKeyForNDraw) {
            gameKey(&snake);
            keyCycleCount = 0;
        }
        // time process
        if(timeCycleCount++ >= snake.conf.timeoutForNDraw) {
            gameTime(&snake);
            timeCycleCount = 0;
        }
        // draw process
        gameDraw(&snake);
        if(snake.state == game_state_exit) {
            break;
        }
        utilSleep(snake.conf.drawMS);
    }
    freeDisplay();
    return status;
}
