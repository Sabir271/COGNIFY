#include "UIStyle.h"
#include <string>

using namespace std;

//--------------------------------------------------
// Generate number
//--------------------------------------------------
static string GenerateNumber(int digits)
{
    string num = "";

    for (int i = 0; i < digits; i++)
    {
        if (i == 0)
            num += to_string(GetRandomValue(1, 9));
        else
            num += to_string(GetRandomValue(0, 9));
    }

    return num;
}

//--------------------------------------------------
// Run as callable from hub
//--------------------------------------------------
void RunNumberMemory()
{
    enum GameState { SHOW_NUMBER, USER_INPUT, GAME_OVER };

    GameState state = SHOW_NUMBER;

    int level = 1;
    int score = 0;

    string targetNumber = GenerateNumber(level);
    string userInput = "";

    float timer = 0.0f;
    const float displayTime = 3.0f;

    Rectangle restartButton = {0, 0, 0, 0};

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_F11)) ToggleAppFullscreen();
        //--------------------------------------------------
        // UPDATE
        //--------------------------------------------------
        float dt = GetFrameTime();

        switch (state)
        {
        case SHOW_NUMBER:
        {
            timer += dt;
            if (timer >= displayTime)
            {
                timer = 0;
                state = USER_INPUT;
            }
            break;
        }

        case USER_INPUT:
        {
            int key = GetCharPressed();

            while (key > 0)
            {
                if (key >= '0' && key <= '9')
                    userInput += (char)key;

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !userInput.empty())
                userInput.pop_back();

            if (IsKeyPressed(KEY_ENTER))
            {
                if (userInput == targetNumber)
                {
                    level++;
                    score += 10;

                    targetNumber = GenerateNumber(level);
                    userInput.clear();

                    timer = 0;
                    state = SHOW_NUMBER;
                }
                else
                {
                    state = GAME_OVER;
                }
            }
            break;
        }

        case GAME_OVER:
        {
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, restartButton) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                level = 1;
                score = 0;

                targetNumber = GenerateNumber(level);
                userInput.clear();

                timer = 0;
                state = SHOW_NUMBER;
            }
            break;
        }
        }

        //--------------------------------------------------
        // DRAW
        //--------------------------------------------------
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        restartButton = { sw * 0.40f, sh * 0.76f, sw * 0.20f, sh * 0.11f };

        BeginDrawing();
        Color bgTop = {12, 18, 36, 255};
        Color bgBottom = {28, 44, 82, 255};
        DrawRectangleGradientV(0, 0, sw, sh, bgTop, bgBottom);
        DrawCircle(sw * 0.20f, sh * 0.24f, sw * 0.11f, {255, 255, 255, 12});
        DrawCircle(sw * 0.86f, sh * 0.18f, sw * 0.075f, {255, 255, 255, 10});

        // Header area (centered)
        Rectangle headerPanel = { sw * 0.08f, sh * 0.04f, sw * 0.84f, sh * 0.14f };
        DrawRectangleRounded(headerPanel, 0.30f, 20, {255, 255, 255, 18});
        DrawRectangleRoundedLines(headerPanel, 0.30f, 20, {255, 255, 255, 50});

        DrawText("NUMBER MEMORY", sw / 2 - MeasureText("NUMBER MEMORY", 34) / 2, sh * 0.08f, 34, Color{245, 247, 255, 255});
        DrawText("Memorize and recall longer numbers.", sw / 2 - MeasureText("Memorize and recall longer numbers.", 18) / 2, sh * 0.135f, 18, Color{210, 220, 240, 200});

        // Stats (right aligned inside header)
        DrawText(TextFormat("Level: %d", level), sw * 0.82f - MeasureText(TextFormat("Level: %d", level), 20), sh * 0.08f, 20, Color{225, 230, 255, 255});
        DrawText(TextFormat("Score: %d", score), sw * 0.82f - MeasureText(TextFormat("Score: %d", score), 20), sh * 0.115f, 20, Color{225, 230, 255, 255});

        // Small info strip under header (centered)
        Rectangle infoBox = { sw * 0.25f, sh * 0.21f, sw * 0.50f, sh * 0.08f };
        DrawRectangleRounded(infoBox, 0.20f, 18, {255, 255, 255, 18});
        DrawRectangleRoundedLines(infoBox, 0.20f, 18, {255, 255, 255, 60});
        DrawText(TextFormat("Next recall in %.1f seconds", displayTime - timer), sw / 2 - MeasureText(TextFormat("Next recall in %.1f seconds", displayTime - timer), 20) / 2, infoBox.y + infoBox.height * 0.18f, 20, Color{180, 205, 245, 255});

        DrawText("Tip: Group digits visually for faster recall.", sw / 2 - MeasureText("Tip: Group digits visually for faster recall.", 16) / 2, sh * 0.31f, 16, Color{200, 210, 235, 220});

        switch (state)
        {
        case SHOW_NUMBER:
        {
            // Center the card and make content balanced
            Rectangle card = { sw * 0.18f, sh * 0.40f, sw * 0.64f, sh * 0.28f };
            DrawRectangleRounded(card, 0.28f, 24, {66, 96, 176, 200});
            DrawRectangleRoundedLines(card, 0.28f, 24, {255, 255, 255, 90});

            DrawText("Memorize", sw / 2 - MeasureText("Memorize", 26) / 2, card.y + sh * 0.02f, 26, Color{230, 240, 255, 255});

            int w = MeasureText(targetNumber.c_str(), 72);
            DrawText(targetNumber.c_str(), sw / 2 - w / 2, card.y + card.height * 0.38f, 72, Color{245, 245, 245, 255});

            DrawText(TextFormat("Time Remaining: %.1f s", displayTime - timer), sw / 2 - MeasureText(TextFormat("Time Remaining: %.1f s", displayTime - timer), 20) / 2, card.y + card.height * 0.78f, 20, Color{235, 245, 255, 255});
            break;
        }

        case USER_INPUT:
        {
            Rectangle card = { sw * 0.18f, sh * 0.38f, sw * 0.64f, sh * 0.30f };
            DrawRectangleRounded(card, 0.28f, 24, {42, 66, 118, 200});
            DrawRectangleRoundedLines(card, 0.28f, 24, {255, 255, 255, 90});

            DrawText("Enter the number", sw / 2 - MeasureText("Enter the number", 28) / 2, card.y + sh * 0.02f, 28, Color{220, 230, 250, 255});

            Rectangle inputField = { sw * 0.28f, card.y + card.height * 0.45f, sw * 0.44f, sh * 0.10f };
            DrawRectangleRounded(inputField, 0.22f, 18, {255, 255, 255, 18});
            DrawRectangleRoundedLines(inputField, 0.22f, 18, {255, 255, 255, 90});
            DrawText(userInput.c_str(), inputField.x + inputField.width * 0.06f, inputField.y + (inputField.height * 0.20f), 40, SKYBLUE);

            DrawText("Press ENTER to submit", sw / 2 - MeasureText("Press ENTER to submit", 20) / 2, card.y + card.height * 0.82f, 20, Color{190, 200, 220, 215});
            break;
        }

        case GAME_OVER:
        {
            Rectangle overlay = { sw * 0.22f, sh * 0.30f, sw * 0.56f, sh * 0.38f };
            DrawRectangleRounded(overlay, 0.30f, 24, {255, 255, 255, 18});
            DrawRectangleRoundedLines(overlay, 0.30f, 24, {255, 255, 255, 120});

            DrawText("GAME OVER", sw / 2 - MeasureText("GAME OVER", 46) / 2, overlay.y + sh * 0.03f, 46, Color{240, 98, 120, 255});
            DrawText(TextFormat("Correct: %s", targetNumber.c_str()), sw / 2 - MeasureText(TextFormat("Correct: %s", targetNumber.c_str()), 24) / 2, overlay.y + sh * 0.16f, 24, Color{245, 245, 245, 255});
            DrawText(TextFormat("Final Level: %d", level), sw / 2 - MeasureText(TextFormat("Final Level: %d", level), 24) / 2, overlay.y + sh * 0.24f, 24, Color{235, 245, 255, 255});
            DrawText(TextFormat("Final Score: %d", score), sw / 2 - MeasureText(TextFormat("Final Score: %d", score), 24) / 2, overlay.y + sh * 0.32f, 24, Color{235, 245, 255, 255});

            Vector2 mouse = GetMousePosition();
            bool hover = CheckCollisionPointRec(mouse, restartButton);

            // Keep restart button centered and visually prominent
            restartButton = { sw * 0.375f, overlay.y + overlay.height * 0.62f, sw * 0.25f, sh * 0.10f };
            DrawRectangleRounded(restartButton, 0.24f, 16, hover ? Color{71, 138, 213, 255} : Color{88, 160, 229, 255});
            DrawRectangleRoundedLines(restartButton, 0.24f, 16, Color{255, 255, 255, 120});
            DrawText("RESTART", restartButton.x + restartButton.width * 0.22f, restartButton.y + restartButton.height * 0.22f, 28, Color{17, 17, 24, 255});
            break;
        }
        }

        EndDrawing();
    }

    return;
}
