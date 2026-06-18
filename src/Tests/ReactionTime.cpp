#include "raylib.h"

enum GameState {
    WAITING,
    READY,
    TOO_EARLY,
    RESULT
};

void RunReactionTime() {
    SetWindowSize(800, 600);

    GameState state = WAITING;

    float waitTime = GetRandomValue(2, 5);
    float timer = 0.0f;

    double startTime = 0.0;
    double reactionTime = 0.0;

    // 🎨 Store random color
    Color randomColor = GREEN;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
        float dt = GetFrameTime();
        timer += dt;

        // LOGIC
        if (state == WAITING) {
            if (timer >= waitTime) {
                state = READY;
                startTime = GetTime();

                // 🎨 Generate random color
                randomColor = {
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255
                };
            }

            if (IsKeyPressed(KEY_SPACE)) {
                state = TOO_EARLY;
            }
        }
        else if (state == READY) {
            if (IsKeyPressed(KEY_SPACE)) {
                reactionTime = (GetTime() - startTime) * 1000.0;
                state = RESULT;
            }
        }
        else if (state == TOO_EARLY || state == RESULT) {
            if (IsKeyPressed(KEY_R)) {
                state = WAITING;
                timer = 0;
                waitTime = GetRandomValue(2, 5);
            }
        }

        // DRAW
        BeginDrawing();
        Color bg = {17, 17, 24, 255};
        ClearBackground(bg);

        DrawText("Press ESC to Exit to Main Menu | F11 for Fullscreen", 20, 10, 14, Color{120, 120, 120, 255});

        if (state == WAITING) {
            DrawText("Wait for color...", 260, 250, 20, Color{245, 245, 245, 255});
        }
        else if (state == READY) {
            DrawRectangle(0, 0, 800, 600, randomColor);
            DrawText("PRESS SPACE!", 280, 250, 20, BLACK);
        }
        else if (state == TOO_EARLY) {
            DrawText("Too Early! Press R to retry", 180, 250, 20, Color{190, 33, 61, 255});
        }
        else if (state == RESULT) {
            DrawText(TextFormat("Reaction Time: %.2f ms", reactionTime), 200, 250, 20, Color{253, 249, 0, 255});
            DrawText("Press R to try again", 250, 300, 20, Color{120, 120, 120, 255});
        }

        EndDrawing();
        }

        return;
    }