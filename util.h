#ifndef __UTIL_H
#define __UTIL_H
#include "std.h"

int random(int min, int max);
et utilSleep(int ms);
et utilReadKey(UINT16* scanCode, CHAR16* unicode);
et utilReadLastKey(UINT16* scanCode, CHAR16* unicode);
et utilReadKeyBlock(UINT16* scanCode, CHAR16* unicode);
et initDisplay();
void freeDisplay();
void getDisplayResolution(int *width, int *height);
et listDisplayMode();
et fillColor(pixel pix);
et drawRect(int x1, int x2, int y1, int y2, pixel pix);
et drawRectWithSize(int x1, int y1, int sizeX, int sizeY, pixel pix);
et drawCircle(int x, int y, int r, pixel pix);
et drawDiamond(int x, int y, int r, pixel pix);
et drawString(int x, int y, CHAR16* str, int size, pixel pix);

#endif