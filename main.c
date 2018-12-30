#include <stdio.h>
#include "std.h"
#include "game.h"
#include "test.h"

int main() {
	et status;
	Print(L"Start\n");
	status = TestKeyCycle();
	Print(L"\n");
	status = TestDisplay();
	Print(L"Finish\n");
	return EFI_SUCCESS;
}

