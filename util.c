#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <Protocol/HiiFont.h>

static EFI_GRAPHICS_OUTPUT_PROTOCOL *display;
static EFI_HII_FONT_PROTOCOL *hiiFont;
static pixel* bltBuffer;
static UINT32 hRes, vRes;

static int round(double d) {
    return d > 0 ? (int)(d + 0.5) : (int)(d - 0.5);
}

int random(int min, int max) {
    return rand() * (max - min) + min;
}

et utilSleep(int ms) {
	// timer unit is 100ns in UEFI
	et status;
	EFI_EVENT myEvent;
	UINTN index;
	status = gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (void*)NULL, &myEvent);
	status = gBS->SetTimer(myEvent, TimerRelative, ms * 1000 * 10);
	status = gBS->WaitForEvent(1, &myEvent, &index);
	return status;
}

// non-blocking read key
et utilReadKey(UINT16* scanCode, CHAR16* unicode) {
    et status;
    EFI_INPUT_KEY key;
    status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
    if(scanCode)
        *scanCode = key.ScanCode;
    if(unicode)
        *unicode = key.UnicodeChar;
    return status;
}

// blocking read key
et utilReadKeyBlock(UINT16* scanCode, CHAR16* unicode) {
    UINTN index;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &index);
    return utilReadKey(scanCode, unicode);
}

// read key until no buffer
et utilReadLastKey(UINT16* scanCode, CHAR16* unicode) {
    et status;
    UINT16 scan1, scan2;
    CHAR16 uni1, uni2;
    status = utilReadKey(&scan1, &uni1);
    status = utilReadKey(&scan2, &uni2);
    while(scan2 || uni2) {
        scan1 = scan2;
        uni1 = uni2;
        status = utilReadKey(&scan2, &uni2);
    }
    *scanCode = scan1;
    *unicode = uni1;
    return status;
}

// initalize display 
et initDisplay() {
    et status;
    status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (void**)&display);
    if(EFI_ERROR(status))
        return status;
    status = gBS->LocateProtocol(&gEfiHiiFontProtocolGuid, NULL, (void**)&hiiFont);
    if(EFI_ERROR(status)) 
        return status;
    hRes = display->Mode->Info->HorizontalResolution;
    vRes = display->Mode->Info->VerticalResolution;
    int bufferSize = hRes * vRes * sizeof(pixel);
    status = gBS->AllocatePool(EfiBootServicesData, bufferSize, (void**)&bltBuffer);
    return status;
}

void freeDisplay() {
    gBS->FreePool(bltBuffer);
}

void getDisplayResolution(int *width, int *height) {
    *width = hRes;
    *height = vRes;
}

// print first 15 supported display mode P266
et listDisplayMode() {
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    et status;
    UINTN infoSize;
    for(UINT32 i = 0; i < (display->Mode->MaxMode > 15 ? 15 : display->Mode->MaxMode); i++) {
        Print(L"Mode %d ", i);
        status = display->QueryMode(display, i, &infoSize, &info);
        if(EFI_ERROR(status)) {
            Print(L"QuerMode %r\n", status);
            break;
        } else {
            Print(L"version %x, width: %d, height: %d, pixel format: %d\n",
                info->Version, info->HorizontalResolution, info->VerticalResolution, info->PixelFormat);
        }
        gBS->FreePool(info);
    }
    return status;
}

// fill with color
et fillColor(pixel pix) {
    et status;
    status = display->Blt(display, &pix, EfiBltVideoFill, 0, 0, 0, 0, hRes, vRes, 0);
    return status;
}

// draw rectangle with color [(x1, y1), (x2, y2)]
et drawRect(int x1, int y1, int x2, int y2, pixel pix) {
    et status;
    status = display->Blt(display, &pix, EfiBltVideoFill, 0, 0, x1, y1, x2 - x1 + 1, y2 - y1 + 1, 0);
    return status;
}

// draw rectangle with color [(x1, y1), (x1 + sizeX, y1 + sizeY))
et drawRectWithSize(int x1, int y1, int sizeX, int sizeY, pixel pix) {
    et status;
    status = display->Blt(display, &pix, EfiBltVideoFill, 0, 0, x1, y1, sizeX, sizeY, 0);
    return status;
}

et drawCircle(int x, int y, int r, pixel pix) {
    et status;
    int y1 = y - r;
    int y2 = y + r;
    for(int yi = y1; yi <= y2; yi++) {
        int dx = round(sqrt(r * r - (yi - y) * (yi - y))); 
        status = drawRectWithSize(x - dx, yi, dx << 1, 1, pix);
    }
    return status;
}

et drawDiamond(int x, int y, int r, pixel pix) {
    et status;
    int y1 = y - r;
    int y2 = y + r;
    for(int yi = y1; yi <= y2; yi++) {
        int dx = r - abs(yi - y);
        status = drawRectWithSize(x - dx, yi, dx << 1, 1, pix);
    }
    return status;
}

// "Gameboy Color" - I want colorful string
// transparent at default, size cannot work now
et drawString(int x, int y, CHAR16* str, int size, pixel color) {
    et status;
    static EFI_FONT_DISPLAY_INFO fontInfo = {
        {0x00, 0x00, 0x00, 0x00}, // foreground color
        {0xFF, 0xFF, 0x00, 0x00}, // background color
        EFI_FONT_INFO_ANY_FONT,   // font mask
        {EFI_HII_FONT_STYLE_NORMAL, 0, {L'0'}} // font style, font size, font name
    };

    // debugging size and style I still can't let it work
    // EFI_FONT_DISPLAY_INFO *pfi;
    // status = hiiFont->GetFontInfo(hiiFont, NULL, NULL, &pfi, NULL);
    // Print(L"style: %d size: %d name: %s \n", pfi->FontInfo.FontStyle, pfi->FontInfo.FontSize, pfi->FontInfo.FontName);
    // Print(L"mask: %x \n", pfi->FontInfoMask);
    // pfi->ForegroundColor = color;
    // pfi->FontInfo.FontStyle = EFI_HII_FONT_STYLE_UNDERLINE;
    // pfi->FontInfo.FontSize = (UINT16)size;
    // pfi->FontInfoMask = EFI_FONT_INFO_RESIZE | EFI_FONT_INFO_RESTYLE;

    fontInfo.ForegroundColor = color;

    // transparent works now
    // After reading HiiFont.h I found that transparent option said it uses color in buffer to paint,
    // then I tried that, yeah that never worked transparent worked on buffer
    // then I copied buffer to screen to draw transparent characters, it works
    EFI_IMAGE_OUTPUT buffer;
    buffer.Height = vRes;
    buffer.Width = hRes;
    buffer.Image.Bitmap = bltBuffer;
    status = display->Blt(display, bltBuffer, EfiBltVideoToBltBuffer, x, y, 0, 0, hRes - x, vRes - y, hRes*sizeof(pixel));
    EFI_IMAGE_OUTPUT* pBuffer = &buffer;
    status = hiiFont->StringToImage(hiiFont, 
        EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_TRANSPARENT,
        str, &fontInfo, &pBuffer, 0, 0, NULL, NULL, NULL);
    status = display->Blt(display, bltBuffer, EfiBltBufferToVideo, 0, 0, x, y, hRes - x, vRes - y, hRes*sizeof(pixel));
    // gBS->FreePool(pfi);
    return status;
}

