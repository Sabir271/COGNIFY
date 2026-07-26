#include "UIStyle.h"
#include <cstdio>

enum GameState
{
    WAITING,
    READY,
    TOO_EARLY,
    RESULT
};

// Expose as RunReactionTime for the hub
void RunReactionTime()
{
    const int screenWidth = 1200;
    const int screenHeight = 800;

    // Use SetWindowSize so we don't reinitialize the window (hub already created it)
    SetWindowSize(screenWidth, screenHeight);
    SetTargetFPS(60);

    GameState state = WAITING;

    float waitTime = GetRandomValue(2, 5);
    float timer = 0.0f;

    double startTime = 0.0;
    double reactionTime = 0.0;

    // Random color and key
    Color randomColor = GREEN;
    KeyboardKey targetKey = KEY_A;
    char targetLetter = 'A';

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        timer += dt;

        // ---------------- GAME LOGIC ----------------
        if (state == WAITING)
        {
            // Pressing any key before the signal = Too Early
            if (GetKeyPressed() != 0)
            {
                state = TOO_EARLY;
            }

            if (timer >= waitTime)
            {
                state = READY;
                startTime = GetTime();

                // Random background color
                randomColor = {
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    (unsigned char)GetRandomValue(50, 255),
                    255
                };

                // Random choice from W, A, S, D only
                const char choices[4] = {'W','A','S','D'};
                int idx = GetRandomValue(0, 3);
                targetLetter = choices[idx];
                switch (targetLetter)
                {
                    case 'W': targetKey = KEY_W; break;
                    case 'A': targetKey = KEY_A; break;
                    case 'S': targetKey = KEY_S; break;
                    case 'D': targetKey = KEY_D; break;
                    default: targetKey = KEY_W; break;
                }
            }
        }
        else if (state == READY)
        {
            int key = GetKeyPressed();

            if (key != 0)
            {
                if (key == targetKey)
                {
                    reactionTime = (GetTime() - startTime) * 1000.0;
                    state = RESULT;
                }
                else
                {
                    state = TOO_EARLY;
                }
            }
        }
        else if (state == TOO_EARLY || state == RESULT)
        {
            if (IsKeyPressed(KEY_R))
            {
                state = WAITING;
                timer = 0.0f;
                waitTime = GetRandomValue(2, 5);
            }
        }

        // allow returning to hub
        if (IsKeyPressed(KEY_ESCAPE)) break;

        // ---------------- DRAW ----------------
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        BeginDrawing();
        Color bgTop = {14, 18, 34, 255};
        Color bgBottom = {28, 40, 68, 255};
        DrawRectangleGradientV(0, 0, sw, sh, bgTop, bgBottom);
        DrawCircle(sw * 0.88f, sh * 0.16f, sw * 0.07f, {255, 255, 255, 12});
        DrawCircle(sw * 0.14f, sh * 0.82f, sw * 0.08f, {255, 255, 255, 10});

        float margin = sw * 0.05f;
        Rectangle header = { margin, sh * 0.05f, sw - margin * 2.0f, sh * 0.14f };
        DrawRectangleRounded(header, 0.26f, 18, {255, 255, 255, 18});
        DrawRectangleRoundedLines(header, 0.26f, 18, {255, 255, 255, 70});
        DrawText("REACTION TIME TRAINER", sw / 2 - MeasureText("REACTION TIME TRAINER", 32) / 2, header.y + sh * 0.02f, 32, WHITE);
        DrawText("Stay calm, wait for the color, then react fast.", sw / 2 - MeasureText("Stay calm, wait for the color, then react fast.", 20) / 2, header.y + sh * 0.08f, 20, Color{200, 215, 240, 210});

        DrawText("Press ESC to Exit to Main Menu", sw * 0.03f, sh * 0.03f, 14, Color{160, 170, 190, 255});

        if (state == WAITING)
        {
            Rectangle board = { sw * 0.20f, sh * 0.32f, sw * 0.60f, sh * 0.20f };
            DrawRectangleRounded(board, 0.28f, 20, {28, 38, 64, 220});
            DrawRectangleRoundedLines(board, 0.28f, 20, {255, 255, 255, 50});

            const char *msg1 = "WAIT FOR THE COLOR";
            const char *msg2 = "Do NOT press any key";

            DrawText(msg1,
                     sw / 2 - MeasureText(msg1, 40) / 2,
                     board.y + board.height * 0.30f,
                     40,
                     WHITE);

            DrawText(msg2,
                     sw / 2 - MeasureText(msg2, 25) / 2,
                     board.y + board.height * 0.65f,
                     25,
                     Color{200, 210, 230, 255});
        }
        else if (state == READY)
        {
            DrawRectangle(0, 0, sw, sh, randomColor);
            Rectangle panel = { sw * 0.14f, sh * 0.20f, sw * 0.72f, sh * 0.60f };
            DrawRectangleRounded(panel, 0.30f, 24, {0, 0, 0, 170});
            DrawRectangleRoundedLines(panel, 0.30f, 24, {255, 255, 255, 80});

            const char *msg = "PRESS THIS KEY";
            DrawText(msg,
                     sw / 2 - MeasureText(msg, 40) / 2,
                     panel.y + sh * 0.03f,
                     40,
                     WHITE);

            char keyText[4] = {targetLetter, '\0'};
            DrawText(keyText,
                     sw / 2 - MeasureText(keyText, 120) / 2,
                     panel.y + sh * 0.18f,
                     120,
                     WHITE);

            DrawText("Stay focused and press the correct letter quickly.",
                     sw / 2 - MeasureText("Stay focused and press the correct letter quickly.", 20) / 2,
                     panel.y + panel.height * 0.75f,
                     20,
                     Color{230, 230, 230, 230});
        }
        else if (state == TOO_EARLY)
        {
            Rectangle panel = { sw * 0.18f, sh * 0.30f, sw * 0.64f, sh * 0.26f };
            DrawRectangleRounded(panel, 0.28f, 24, {255, 255, 255, 22});
            DrawRectangleRoundedLines(panel, 0.28f, 24, {255, 255, 255, 90});

            const char *msg1 = "TOO EARLY!";
            const char *msg2 = "Press R to Play Again";

            DrawText(msg1,
                     sw / 2 - MeasureText(msg1, 60) / 2,
                     panel.y + sh * 0.06f,
                     60,
                     RED);

            DrawText(msg2,
                     sw / 2 - MeasureText(msg2, 30) / 2,
                     panel.y + panel.height * 0.65f,
                     30,
                     WHITE);
        }
        else if (state == RESULT)
        {
            Rectangle panel = { sw * 0.18f, sh * 0.30f, sw * 0.64f, sh * 0.26f };
            DrawRectangleRounded(panel, 0.28f, 24, {255, 255, 255, 22});
            DrawRectangleRoundedLines(panel, 0.28f, 24, {255, 255, 255, 90});

            const char *title = "YOUR REACTION TIME";
            DrawText(title,
                     sw / 2 - MeasureText(title, 40) / 2,
                     panel.y + sh * 0.04f,
                     40,
                     YELLOW);

            char resultText[64];
            sprintf(resultText, "%.2f ms", reactionTime);
            DrawText(resultText,
                     sw / 2 - MeasureText(resultText, 60) / 2,
                     panel.y + panel.height * 0.45f,
                     60,
                     GREEN);

            const char *msg = "Press R to Play Again";
            DrawText(msg,
                     sw / 2 - MeasureText(msg, 30) / 2,
                     panel.y + panel.height * 0.80f,
                     30,
                     WHITE);
        }

        EndDrawing();
    }

    // Do not call CloseWindow() here — the main hub manages the window lifecycle.
}

