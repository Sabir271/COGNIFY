#include "UIStyle.h"

Font uiFont = { 0 };
static bool uiFontLoaded = false;

Font LoadUIFont() {
    const char* fontPaths[] = {
        "resources/Roboto-Regular.ttf",
        "resources/SegoeUI.ttf",
        "resources/Arial.ttf",
        "C:/Windows/Fonts/SegoeUI.ttf",
        "C:/Windows/Fonts/Arial.ttf",
        "C:/Windows/Fonts/Verdana.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    };

    for (int i = 0; i < sizeof(fontPaths) / sizeof(fontPaths[0]); i++) {
        if (FileExists(fontPaths[i])) {
            uiFont = LoadFontEx(fontPaths[i], 64, 0, 0);
            uiFontLoaded = true;
            return uiFont;
        }
    }

    uiFont = GetFontDefault();
    uiFontLoaded = false;
    return uiFont;
}

void UnloadUIFont() {
    if (uiFontLoaded && uiFont.texture.id != 0) {
        UnloadFont(uiFont);
        uiFont = { 0 };
        uiFontLoaded = false;
    }
}

float MeasureTextUI(const char* text, float fontSize) {
    return MeasureTextEx(uiFont, text, fontSize, 0).x;
}

void DrawTextUI(const char* text, float x, float y, float fontSize, Color color) {
    DrawTextEx(uiFont, text, {x, y}, fontSize, 0, color);
}

void DrawTextUI(const char* text, Vector2 position, float fontSize, Color color) {
    DrawTextEx(uiFont, text, position, fontSize, 0, color);
}
