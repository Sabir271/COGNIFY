#pragma once

#include "raylib.h"

extern Font uiFont;

Font LoadUIFont();
void UnloadUIFont();

float MeasureTextUI(const char* text, float fontSize);
void DrawTextUI(const char* text, float x, float y, float fontSize, Color color);
void DrawTextUI(const char* text, Vector2 position, float fontSize, Color color);
void SetAppFullscreen(bool enable);
void ToggleAppFullscreen();
bool IsAppFullscreen();

// Note: avoid defining `MeasureText` to prevent collision with raylib's symbol.
// Use `MeasureTextUI` to measure text using the loaded `uiFont`.

inline void DrawTextCentered(const char* text, Rectangle rect, float fontSize, Color color) {
    float textWidth = MeasureTextUI(text, fontSize);
    float x = rect.x + (rect.width - textWidth) / 2.0f;
    float y = rect.y + (rect.height - fontSize) / 2.0f;
    DrawTextUI(text, x, y, fontSize, color);
}

inline void DrawPanel(Rectangle rect, Color background, Color border, float roundness = 0.24f, int segments = 20) {
    DrawRectangleRounded(rect, roundness, segments, background);
    DrawRectangleRoundedLines(rect, roundness, segments, border);
}

inline void DrawActionButton(Rectangle rect, const char* label, bool hovered, Color base, Color textColor = WHITE) {
    Color fill = hovered ? base : Color{static_cast<unsigned char>(base.r + 16), static_cast<unsigned char>(base.g + 16), static_cast<unsigned char>(base.b + 16), base.a};
    Color border = hovered ? WHITE : Color{255,255,255,100};
    DrawRectangleRounded(rect, 0.24f, 16, fill);
    DrawRectangleRoundedLines(rect, 0.24f, 16, border);
    float textX = rect.x + (rect.width - MeasureTextUI(label, 22)) / 2.0f;
    float textY = rect.y + (rect.height - 22) / 2.0f;
    DrawTextUI(label, textX, textY, 22, textColor);
}

inline void DrawSectionHeader(const char* title, const char* subtitle, Rectangle rect) {
    DrawPanel(rect, Color{255,255,255,18}, Color{255,255,255,60}, 0.28f, 20);
    DrawTextUI(title, rect.x + 24, rect.y + 18, 28, WHITE);
    DrawTextUI(subtitle, rect.x + 24, rect.y + 56, 18, Color{200,220,240,220});
}

inline bool IsMouseHovering(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect);
}
