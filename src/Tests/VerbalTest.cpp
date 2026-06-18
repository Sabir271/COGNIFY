#include "raylib.h"
#include <string>

struct Question
{
    std::string word;
    std::string options[4];
    int correctAnswer;
};

void RunVerbalTest()
{
    const int screenWidth = 900;
    const int screenHeight = 600;

    SetWindowSize(screenWidth, screenHeight);

    Question questions[] =
    {
        {
            "Happy",
            {"Sad", "Joyful", "Angry", "Weak"},
            1
        },
        {
            "Rapid",
            {"Slow", "Fast", "Heavy", "Tiny"},
            1
        },
        {
            "Ancient",
            {"Modern", "Old", "Fresh", "Bright"},
            1
        },
        {
            "Brave",
            {"Cowardly", "Fearful", "Courageous", "Quiet"},
            2
        },
        {
            "Tiny",
            {"Large", "Huge", "Small", "Wide"},
            2
        }
    };

    int totalQuestions = sizeof(questions) / sizeof(questions[0]);

    int currentQuestion = 0;
    int score = 0;
    bool finished = false;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        if (!finished)
        {
            if (IsKeyPressed(KEY_ONE))
            {
                if (questions[currentQuestion].correctAnswer == 0)
                    score++;

                currentQuestion++;
            }

            if (IsKeyPressed(KEY_TWO))
            {
                if (questions[currentQuestion].correctAnswer == 1)
                    score++;

                currentQuestion++;
            }

            if (IsKeyPressed(KEY_THREE))
            {
                if (questions[currentQuestion].correctAnswer == 2)
                    score++;

                currentQuestion++;
            }

            if (IsKeyPressed(KEY_FOUR))
            {
                if (questions[currentQuestion].correctAnswer == 3)
                    score++;

                currentQuestion++;
            }

            if (currentQuestion >= totalQuestions)
            {
                finished = true;
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!finished)
        {
            DrawText("VERBAL COGNITIVE SKILL TEST", 220, 40, 30, DARKBLUE);

            DrawText(
                TextFormat("Question %d of %d",
                currentQuestion + 1,
                totalQuestions),
                50, 100, 25, BLACK);

            DrawText(
                TextFormat("Choose a synonym for: %s",
                questions[currentQuestion].word.c_str()),
                50, 170, 30, MAROON);

            for (int i = 0; i < 4; i++)
            {
                DrawRectangle(70, 250 + i * 70, 350, 50, LIGHTGRAY);

                DrawText(
                    TextFormat("%d. %s",
                    i + 1,
                    questions[currentQuestion].options[i].c_str()),
                    90, 265 + i * 70, 22, BLACK);
            }

            DrawText(
                "Press 1, 2, 3 or 4 to answer",
                50, 540, 20, DARKGRAY);
        }
        else
        {
            DrawText("TEST COMPLETED", 280, 180, 40, DARKGREEN);

            DrawText(
                TextFormat("Your Score: %d / %d",
                score,
                totalQuestions),
                300, 270, 35, BLACK);

            if (score >= 4)
            {
                DrawText("Excellent verbal reasoning!",
                         250, 350, 28, BLUE);
            }
            else if (score >= 2)
            {
                DrawText("Good effort!",
                         330, 350, 28, ORANGE);
            }
            else
            {
                DrawText("Keep practicing vocabulary.",
                         240, 350, 28, RED);
            }
        }

        EndDrawing();
    }

    return;
}