#include "test.h"
#include "util.h"

EFI_STATUS TestInput() {
	EFI_STATUS status;
	CHAR16 strBuffer[3] = { 0 };
	Print(L"Please press a key:\n");
	status = utilReadKeyBlock(NULL, strBuffer);
	strBuffer[1] = '\n';
	gST->ConOut->OutputString(gST->ConOut, strBuffer);
	return status;
}

EFI_STATUS TestSleep() {
	int n = 0;
	for(n = 0; n < 10; n++) {
		Print(L"%d", n);
		utilSleep(200);
	}
	return EFI_SUCCESS;
}

EFI_STATUS TestKeyCycle() {
    EFI_STATUS status;
    UINT16 scanCode;
    CHAR16 unicode;
	int n = 0;
    while(n++ < 5) {
        status = utilReadLastKey(&scanCode, &unicode);
		if(unicode == 0) {
			if(scanCode == 0) {
				Print(L"<nop>");
			} else {
				Print(L"%d", scanCode);
			}
		} else {
			Print(L"%c", unicode);
		}
        utilSleep(200);
    }
    return status;
}

EFI_STATUS TestDisplay() {
	EFI_STATUS status;
	status = initDisplay();
	status = listDisplayMode();
	pixel white = { 255, 255, 255, 0 };
	status = fillColor(white);
	pixel red = { 0, 0, 255, 0 };
	pixel blue = { 255, 0, 0, 0 };
	status = drawRect(10, 10, 220, 120, red);
	status = drawCircle(140, 240, 100, red);
	status = drawDiamond(240, 340, 70, red);
	status = drawString(250, 100, L"Hello World!", 70, red);
	status = utilSleep(5000);
	freeDisplay();
	return status;
}
