#include <stdio.h>
#include "std.h"
#include "game.h"
#include "test.h"

int main() {
	et status;
	Print(L"Start\n");
	// status = TestKeyCycle();
	// Print(L"\n");
	// status = TestDisplay();
	gameCycle();
	// int w = 800, h = 600, r = 40;
	// pixel wall = {255, 0, 255, 0};
	// initDisplay();
	// Print(L"Wall.%d,%d,%d\n", w, h, r);
    // drawRect(r, 0, w - 1, r, wall);
    // drawRect(w-r, r, w - 1, h - 1, wall);
    // drawRect(0, 0, r, h-r, wall);
    // drawRect(0, h-r, w-r, h - 1, wall);
	// freeDisplay();
	Print(L"Finish\n");
	return EFI_SUCCESS;
}

