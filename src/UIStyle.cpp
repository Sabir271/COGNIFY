#include "UIStyle.h"

Font uiFont = { 0 };
static bool uiFontLoaded = false;

namespace {
    struct SavedWindowState {
        bool hasSavedState;
        int x;
        int y;
        int width;
        int height;
    };

    SavedWindowState g_savedWindowState = {false, 0, 0, 1200, 800};

    void SaveWindowedState() {
        g_savedWindowState.hasSavedState = true;
        g_savedWindowState.x = GetWindowPosition().x;
        g_savedWindowState.y = GetWindowPosition().y;
        g_savedWindowState.width = GetScreenWidth();
        g_savedWindowState.height = GetScreenHeight();
    }
}

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

void SetAppFullscreen(bool enable) {
    if (enable) {
        if (!IsWindowFullscreen()) {
            SaveWindowedState();

            int monitor = GetCurrentMonitor();
            int monitorWidth = GetMonitorWidth(monitor);
            int monitorHeight = GetMonitorHeight(monitor);
            Vector2 monitorPos = GetMonitorPosition(monitor);

            SetWindowSize(monitorWidth, monitorHeight);
            SetWindowPosition((int)monitorPos.x, (int)monitorPos.y);
            SetWindowState(FLAG_FULLSCREEN_MODE);
        }
        return;
    }

    if (IsWindowFullscreen()) {
        ClearWindowState(FLAG_FULLSCREEN_MODE);
        if (g_savedWindowState.hasSavedState) {
            SetWindowSize(g_savedWindowState.width, g_savedWindowState.height);
            SetWindowPosition(g_savedWindowState.x, g_savedWindowState.y);
        }
    }
}

void ToggleAppFullscreen() {
    SetAppFullscreen(!IsWindowFullscreen());
}

bool IsAppFullscreen() {
    return IsWindowFullscreen();
}
