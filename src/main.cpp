#include "UIStyle.h"
#include "Tests/Tests.h"
#include <time.h>
#include <stdlib.h>

namespace {
    struct ModuleCard {
        Rectangle rect;
        Color baseColor;
        Color hoverColor;
        const char* title;
        const char* subtitle;
        const char* badge;
    };

    void DrawCard(Rectangle rect, Color fill, Color border, const char* title, const char* subtitle, const char* badge, bool hovered) {
        Color shadow = {0, 0, 0, 60};
        Color cardFill = hovered ? fill : Color{static_cast<unsigned char>(fill.r + 10), static_cast<unsigned char>(fill.g + 10), static_cast<unsigned char>(fill.b + 10), 220};
        Color badgeFill = hovered ? Color{255, 255, 255, 210} : Color{255, 255, 255, 160};

        DrawRectangleRounded({rect.x + 4, rect.y + 4, rect.width, rect.height}, 0.24f, 16, shadow);
        DrawRectangleRounded(rect, 0.24f, 16, cardFill);
        DrawRectangleRoundedLines(rect, 0.24f, 16, border);

        Rectangle badgeBox = {rect.x + 22, rect.y + 22, 36, 36};
        DrawRectangleRounded(badgeBox, 0.3f, 8, badgeFill);
        DrawTextUI(badge, badgeBox.x + 11, badgeBox.y + 7, 18, fill);

        DrawTextUI(title, rect.x + 74, rect.y + 24, 24, WHITE);
        DrawTextUI(subtitle, rect.x + 74, rect.y + 56, 16, {230, 235, 245, 210});
    }
}

int main() {
    const int defaultWidth = 1200;
    const int defaultHeight = 800;

    InitWindow(defaultWidth, defaultHeight, "Raylib Cognitive Brain Hub");
    LoadUIFont();
    SetTargetFPS(60);
    srand(time(NULL));

    ModuleCard cards[5] = {
        {{0, 0, 0, 0}, {93, 123, 255, 255}, {115, 145, 255, 255}, "Typing", "Precision and rhythm", "1"},
        {{0, 0, 0, 0}, {74, 179, 109, 255}, {92, 199, 126, 255}, "Stroop", "Color recognition", "2"},
        {{0, 0, 0, 0}, {255, 171, 64, 255}, {255, 193, 93, 255}, "Reaction", "Fast reflexes", "3"},
        {{0, 0, 0, 0}, {125, 108, 214, 255}, {148, 130, 236, 255}, "Number", "Working memory", "4"},
        {{0, 0, 0, 0}, {214, 92, 178, 255}, {236, 116, 198, 255}, "Verbal", "Word recall", "5"}
    };
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        bool fullscreen = IsWindowFullscreen();
        int margin = 42;
        int heroHeight = screenHeight * 18 / 100;
        int cardWidth = screenWidth * 28 / 100;
        int cardHeight = screenHeight * 14 / 100;
        if (cardWidth < 280) cardWidth = 280;
        if (cardHeight < 100) cardHeight = 100;
        if (cardWidth > 380) cardWidth = 380;
        if (cardHeight > 140) cardHeight = 140;

        Rectangle btnSettings = { screenWidth - 190 - margin, margin, 170, 40 };
        Rectangle hero = { margin, margin, screenWidth - margin * 2, heroHeight };

        float cardSpacingX = (screenWidth - margin * 2 - cardWidth * 2) / 3.0f;
        float cardSpacingY = 20.0f;
        float cardRowY1 = hero.y + hero.height + 30;
        float cardRowY2 = cardRowY1 + cardHeight + cardSpacingY;
        float cardRowY3 = cardRowY2 + cardHeight + cardSpacingY;

        cards[0].rect = { margin + cardSpacingX, cardRowY1, (float)cardWidth, (float)cardHeight };
        cards[1].rect = { margin + cardSpacingX * 2 + cardWidth, cardRowY1, (float)cardWidth, (float)cardHeight };
        cards[2].rect = { margin + cardSpacingX, cardRowY2, (float)cardWidth, (float)cardHeight };
        cards[3].rect = { margin + cardSpacingX * 2 + cardWidth, cardRowY2, (float)cardWidth, (float)cardHeight };
        cards[4].rect = { screenWidth / 2.0f - cardWidth / 2.0f, cardRowY3, (float)cardWidth, (float)cardHeight };

        Vector2 mousePos = GetMousePosition();

        if (CheckCollisionPointRec(mousePos, btnSettings) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            fullscreen = !fullscreen;
            if (fullscreen) ToggleFullscreen();
            else SetWindowSize(defaultWidth, defaultHeight);
        }

        for (int i = 0; i < 5; ++i) {
            bool hovered = CheckCollisionPointRec(mousePos, cards[i].rect);
            if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                switch (i) {
                    case 0: RunTypingTest(); break;
                    case 1: RunStroopTest(); break;
                    case 2: RunReactionTime(); break;
                    case 3: RunNumberMemory(); break;
                    case 4: RunVerbalTest(); break;
                }
            }
        }

        BeginDrawing();
        ClearBackground({12, 16, 30, 255});
        DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), {16, 24, 42, 255}, {30, 42, 74, 255});
        DrawCircle(180, 120, 94, {255, 255, 255, 12});
        DrawCircle(840, 90, 56, {255, 255, 255, 10});
        DrawCircle(870, 230, 32, {255, 255, 255, 16});

        DrawPanel(hero, {255, 255, 255, 18}, {255, 255, 255, 45}, 0.28f, 18);
        DrawText("COGNITIVE PERFORMANCE HUB", hero.x + 28, hero.y + 24, 30, {245, 247, 255, 255});
        DrawText("Modern brain training for focus, memory, and reaction speed.", hero.x + 28, hero.y + 72, 18, {206, 217, 236, 220});

        DrawRectangleRounded({hero.x + 28, hero.y + hero.height - 44, 260, 30}, 0.4f, 10, {95, 125, 255, 140});
        DrawText("Focus • Memory • Speed", hero.x + 40, hero.y + hero.height - 40, 17, WHITE);

        bool overSettings = CheckCollisionPointRec(mousePos, btnSettings);
        DrawActionButton(btnSettings, fullscreen ? "Windowed" : "Fullscreen", overSettings, {44, 48, 64, 255});

        for (int i = 0; i < 5; ++i) {
            bool hovered = CheckCollisionPointRec(mousePos, cards[i].rect);
            DrawCard(cards[i].rect, cards[i].baseColor, {255, 255, 255, 60}, cards[i].title, cards[i].subtitle, cards[i].badge, hovered);
        }

        Rectangle footer = {margin, screenHeight - 56, screenWidth - margin * 2, 36};
        DrawPanel(footer, {255, 255, 255, 16}, {255, 255, 255, 35}, 0.22f, 12);
        DrawText("Breathe. Focus. Train daily.", footer.x + 18, footer.y + 10, 16, {200, 210, 230, 220});
        DrawText("Press ESC to Close Hub Application", screenWidth / 2 - MeasureText("Press ESC to Close Hub Application", 14) / 2, footer.y + 10, 14, {180, 190, 210, 200});

        EndDrawing();
    }

    UnloadUIFont();
    CloseWindow();
    return 0;
}