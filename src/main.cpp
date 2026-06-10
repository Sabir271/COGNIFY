#include "raylib.h"
#include "Tests/Tests.h"
#include <time.h>
#include <stdlib.h>

int main() {
    // Initial setup container
    const int defaultWidth = 800;
    const int defaultHeight = 600;
    
    InitWindow(defaultWidth, defaultHeight, "Raylib Cognitive Brain Hub");
    SetTargetFPS(60);
    srand(time(NULL));

    // Define dashboard click metrics (Only Typing and Stroop retained)
    Rectangle btnTyping = { 250, 220, 300, 60 };
    Rectangle btnStroop = { 250, 320, 300, 60 };

    while (!WindowShouldClose()) {
        // Handle Menu window dimensions resetting after returning from another application state
        if (GetScreenWidth() != defaultWidth || GetScreenHeight() != defaultHeight) {
            SetWindowSize(defaultWidth, defaultHeight);
        }

        Vector2 mousePos = GetMousePosition();

        // Selection Verification
        if (CheckCollisionPointRec(mousePos, btnTyping) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            RunTypingTest();
        }
        if (CheckCollisionPointRec(mousePos, btnStroop) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            RunStroopTest();
        }

        // Render Dashboard
        BeginDrawing();
        ClearBackground({24, 24, 32, 255});

        DrawText("COGNITIVE PERFORMANCE HUB", defaultWidth / 2 - MeasureText("COGNITIVE PERFORMANCE HUB", 32) / 2, 120, 32, GOLD);
        DrawText("Select an evaluation test module below:", defaultWidth / 2 - MeasureText("Select an evaluation test module below:", 18) / 2, 180, 18, LIGHTGRAY);

        // Typing Module UI Button Configuration
        bool overTyping = CheckCollisionPointRec(mousePos, btnTyping);
        DrawRectangleRounded(btnTyping, 0.2f, 4, overTyping ? SKYBLUE : BLUE);
        DrawText("1. Typing Speed Test", btnTyping.x + 35, btnTyping.y + 18, 22, WHITE);

        // Stroop Module UI Button Configuration
        bool overStroop = CheckCollisionPointRec(mousePos, btnStroop);
        DrawRectangleRounded(btnStroop, 0.2f, 4, overStroop ? LIME : GREEN);
        DrawText("2. Stroop Color Test", btnStroop.x + 40, btnStroop.y + 18, 22, WHITE);

        DrawText("Press ESC to Close Hub Application", defaultWidth / 2 - MeasureText("Press ESC to Close Hub Application", 14) / 2, 540, 14, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}