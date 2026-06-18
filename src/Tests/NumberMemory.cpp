#include "raylib.h"
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
    const int screenWidth = 900;
    const int screenHeight = 500;

    SetWindowSize(screenWidth, screenHeight);

    enum GameState { SHOW_NUMBER, USER_INPUT, GAME_OVER };

    GameState state = SHOW_NUMBER;

    int level = 1;
    int score = 0;

    string targetNumber = GenerateNumber(level);
    string userInput = "";

    float timer = 0.0f;
    const float displayTime = 3.0f;

    Rectangle restartButton = {325, 380, 250, 60};

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
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
        BeginDrawing();
        Color bg = {17, 17, 24, 255};
        ClearBackground(bg);

        DrawText("NUMBER MEMORY TEST", 240, 20, 35, Color{253, 249, 0, 255});
        DrawText(TextFormat("Level: %d", level), 30, 80, 25, Color{245, 245, 245, 255});
        DrawText(TextFormat("Score: %d", score), 30, 120, 25, Color{245, 245, 245, 255});

        switch (state)
        {
        case SHOW_NUMBER:
        {
            DrawText("Memorize the number", 280, 120, 30, Color{200, 200, 200, 255});

            int w = MeasureText(targetNumber.c_str(), 70);
            DrawText(targetNumber.c_str(),
                     screenWidth / 2 - w / 2,
                     220,
                     70,
                     Color{245, 245, 245, 255});

            DrawText(
                TextFormat("Time: %.1f", displayTime - timer),
                380,
                340,
                25,
                Color{230, 41, 55, 255});

            break;
        }

        case USER_INPUT:
        {
            DrawText("Enter the number", 320, 120, 30, Color{200, 200, 200, 255});

            DrawRectangleLines(250, 220, 400, 60, Color{245, 245, 245, 255});

            DrawText(userInput.c_str(), 270, 230, 40, SKYBLUE);

            DrawText("Press ENTER to submit", 300, 320, 25, Color{120, 120, 120, 255});

            break;
        }

        case GAME_OVER:
        {
            DrawText("GAME OVER", 300, 100, 60, Color{190, 33, 61, 255});

            DrawText(
                TextFormat("Correct: %s", targetNumber.c_str()),
                270, 200, 30, Color{245, 245, 245, 255});

            DrawText(
                TextFormat("Final Level: %d", level),
                320, 250, 30, Color{253, 249, 0, 255});

            DrawText(
                TextFormat("Final Score: %d", score),
                320, 290, 30, Color{253, 249, 0, 255});

            //--------------------------------------------------
            // Restart Button
            //--------------------------------------------------
            Vector2 mouse = GetMousePosition();

            bool hover = CheckCollisionPointRec(mouse, restartButton);

            DrawRectangleRec(restartButton, hover ? SKYBLUE : LIGHTGRAY);
            DrawRectangleLinesEx(restartButton, 2, Color{45, 45, 61, 255});

            DrawText(
                "RESTART",
                restartButton.x + 70,
                restartButton.y + 18,
                30,
                Color{17, 17, 24, 255});

            break;
        }
        }

        EndDrawing();
    }

    return;
}
