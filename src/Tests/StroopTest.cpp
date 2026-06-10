#include "raylib.h"
#include <stdlib.h>
#include <time.h>

// Global Window Constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int COLOR_COUNT = 5;

// Game States
enum GameState {
    STATE_START,
    STATE_PLAYING,
    STATE_GAMEOVER
};

// ==========================================
// THE ULTIMATE SIMPLE COLOR HELPER
// ==========================================
Color MakeColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

// Simple text mapping
const char* GetColorName(int id) {
    switch (id) {
        case 0: return "RED";
        case 1: return "BLUE";
        case 2: return "GREEN";
        case 3: return "YELLOW";
        case 4: return "PURPLE";
        default: return "UNKNOWN";
    }
}

// Returns custom safe colors using our helper function
Color GetColorValue(int id) {
    switch (id) {
        case 0: return MakeColor(230, 41, 55, 255);   // Red
        case 1: return MakeColor(0, 121, 242, 255);   // Blue
        case 2: return MakeColor(0, 228, 48, 255);    // Green
        case 3: return MakeColor(253, 249, 0, 255);   // Yellow
        case 4: return MakeColor(200, 122, 255, 255); // Purple
        default: return MakeColor(0, 0, 0, 255);      // Black
    }
}

// ==========================================
// 1. BUTTON CLASS
// ==========================================
class ColorButton {
private:
    Rectangle bounds;
    Color buttonColor;
    int colorId;

public:
    ColorButton() {
        bounds.x = 0.0f;
        bounds.y = 0.0f;
        bounds.width = 0.0f;
        bounds.height = 0.0f;
        buttonColor = MakeColor(0, 0, 0, 255); 
        colorId = -1;
    }

    void Setup(float x, float y, float width, float height, int id, Color color) {
        bounds.x = x;
        bounds.y = y;
        bounds.width = width;
        bounds.height = height;
        colorId = id;
        buttonColor = color;
    }

    int GetColorId() { return colorId; }
    Rectangle GetBounds() { return bounds; }

    bool IsClicked(Vector2 mousePos) {
        return (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, bounds));
    }

    void Draw() {
        DrawRectangleRounded(bounds, 0.25f, 4, buttonColor);
        DrawRectangleRoundedLines(bounds, 0.25f, 4, MakeColor(255, 255, 255, 255));
    }
};

// ==========================================
// 2. STROOP GAME CLASS
// ==========================================
class StroopGame {
private:
    ColorButton buttons[COLOR_COUNT];
    int currentWordIdx;  
    int currentColorIdx; 
    int score;
    int totalTrials;
    bool showFeedback;
    bool lastAnswerCorrect;
    float feedbackTimer;

    float timeRemaining;
    GameState currentScene; 

    void NextTrial() {
        currentWordIdx = rand() % COLOR_COUNT;
        currentColorIdx = rand() % COLOR_COUNT;
    }

    const char* GetEvaluationGrade() {
        if (score >= 25) return "GENIUS: Unstoppable Brain Speed!";
        if (score >= 18) return "EXCELLENT: Superb Cognitive Control!";
        if (score >= 10) return "GOOD: Solid Processing Performance.";
        return "PRACTICE: Focus purely on ink color!";
    }

public:
    StroopGame() {
        currentScene = STATE_START; 
        score = 0;
        totalTrials = 0;
        showFeedback = false;
        lastAnswerCorrect = false;
        feedbackTimer = 0.0f;
        timeRemaining = 30.0f;

        float btnWidth = 110.0f;
        float btnHeight = 60.0f;
        float spacing = 20.0f;
        
        float totalWidth = (COLOR_COUNT * btnWidth) + ((COLOR_COUNT - 1) * spacing);
        float startX = (SCREEN_WIDTH - totalWidth) / 2.0f;
        float btnY = 450.0f;

        for (int i = 0; i < COLOR_COUNT; i++) {
            float posX = startX + i * (btnWidth + spacing);
            buttons[i].Setup(posX, btnY, btnWidth, btnHeight, i, GetColorValue(i));
        }
    }

    void StartNewSession() {
        score = 0;
        totalTrials = 0;
        showFeedback = false;
        lastAnswerCorrect = false;
        feedbackTimer = 0.0f;
        timeRemaining = 30.0f; 
        currentScene = STATE_PLAYING;
        NextTrial();
    }

    void Update() {
        switch (currentScene) {
            case STATE_START:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    StartNewSession();
                }
                break;

            case STATE_PLAYING: {  // FIXED: Added opening brace for scope isolation
                timeRemaining -= GetFrameTime();
                if (timeRemaining <= 0.0f) {
                    timeRemaining = 0.0f;
                    currentScene = STATE_GAMEOVER;
                    return;
                }

                if (showFeedback) {
                    feedbackTimer += GetFrameTime();
                    if (feedbackTimer >= 0.35f) { 
                        showFeedback = false;
                        feedbackTimer = 0.0f;
                        NextTrial(); 
                    }
                    return; 
                }

                Vector2 mousePos = GetMousePosition();
                for (int i = 0; i < COLOR_COUNT; i++) {
                    if (buttons[i].IsClicked(mousePos)) {
                        totalTrials++;
                        showFeedback = true;

                        if (buttons[i].GetColorId() == currentColorIdx) {
                            score++;
                            lastAnswerCorrect = true;
                        } else {
                            lastAnswerCorrect = false;
                        }
                        break;
                    }
                }
                break;
            } // FIXED: Added closing brace for scope isolation

            case STATE_GAMEOVER:
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                    StartNewSession();
                }
                break;
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(MakeColor(17, 17, 24, 255)); 

        if (currentScene == STATE_START) {
            // SCENE: WELCOME START SCREEN
            DrawText("STROOP COGNITIVE TEST", SCREEN_WIDTH / 2 - MeasureText("STROOP COGNITIVE TEST", 38) / 2, 140, 38, MakeColor(253, 249, 0, 255));
            DrawLine(100, 200, SCREEN_WIDTH - 100, 200, MakeColor(45, 45, 61, 255));

            const char* rule1 = "1. Words will appear in random colors.";
            const char* rule2 = "2. Ignore what the text spells out completely!";
            const char* rule3 = "3. Click the bottom color button that matches the INK COLOR.";
            const char* rule4 = "4. Race against the 30 second countdown timer clock.";

            DrawText(rule1, 120, 240, 20, MakeColor(245, 245, 245, 255));
            DrawText(rule2, 120, 280, 20, MakeColor(230, 41, 55, 255)); 
            DrawText(rule3, 120, 320, 20, MakeColor(245, 245, 245, 255));
            DrawText(rule4, 120, 360, 20, MakeColor(245, 245, 245, 255));

            DrawLine(100, 420, SCREEN_WIDTH - 100, 420, MakeColor(45, 45, 61, 255));
            DrawText("Press [ENTER] or [SPACE] to Begin", SCREEN_WIDTH / 2 - MeasureText("Press [ENTER] or [SPACE] to Begin", 24) / 2, 470, 24, MakeColor(0, 228, 48, 255));

        } else if (currentScene == STATE_GAMEOVER) {
            // SCENE: EVALUATION SCORE SCREEN
            DrawText("TEST COMPLETE", SCREEN_WIDTH / 2 - MeasureText("TEST COMPLETE", 36) / 2, 100, 36, MakeColor(253, 249, 0, 255));
            DrawLine(150, 160, SCREEN_WIDTH - 150, 160, MakeColor(45, 45, 61, 255));

            float finalAccuracy = (totalTrials > 0) ? ((float)score / totalTrials) * 100.0f : 0.0f;

            const char* finalScoreStr = TextFormat("Correct Answers: %d", score);
            const char* totalAttemptsStr = TextFormat("Total Attempts: %d", totalTrials);
            const char* accuracyStr = TextFormat("Final Accuracy: %.1f%%", finalAccuracy);

            DrawText(finalScoreStr, SCREEN_WIDTH / 2 - MeasureText(finalScoreStr, 24) / 2, 210, 24, MakeColor(245, 245, 245, 255));
            DrawText(totalAttemptsStr, SCREEN_WIDTH / 2 - MeasureText(totalAttemptsStr, 24) / 2, 255, 24, MakeColor(180, 180, 180, 255));
            DrawText(accuracyStr, SCREEN_WIDTH / 2 - MeasureText(accuracyStr, 24) / 2, 300, 24, MakeColor(0, 228, 48, 255));

            const char* evaluationText = GetEvaluationGrade();
            DrawText(evaluationText, SCREEN_WIDTH / 2 - MeasureText(evaluationText, 22) / 2, 380, 22, MakeColor(200, 122, 255, 255));

            DrawLine(150, 440, SCREEN_WIDTH - 150, 440, MakeColor(45, 45, 61, 255));
            DrawText("Press [SPACE] to Try Again", SCREEN_WIDTH / 2 - MeasureText("Press [SPACE] to Try Again", 20) / 2, 480, 20, MakeColor(130, 130, 130, 255));

        } else if (currentScene == STATE_PLAYING) {
            // SCENE: ACTIVE RUNNING GAME LOOP
            DrawText("STROOP TEST", 40, 30, 24, MakeColor(130, 130, 130, 255));
            
            const char* scoreText = TextFormat("Score: %d", score);
            DrawText(scoreText, 240, 30, 24, MakeColor(245, 245, 245, 255));

            float accuracy = (totalTrials > 0) ? ((float)score / totalTrials) * 100.0f : 0.0f;
            const char* accuracyText = TextFormat("Accuracy: %.0f%%", accuracy);
            DrawText(accuracyText, 410, 30, 24, MakeColor(0, 228, 48, 255));

            const char* timerText = TextFormat("Time: %.1fs", timeRemaining);
            Color timerColor = (timeRemaining <= 5.0f) ? MakeColor(230, 41, 55, 255) : MakeColor(253, 249, 0, 255);
            DrawText(timerText, SCREEN_WIDTH - MeasureText(timerText, 24) - 40, 30, 24, timerColor);

            DrawLine(40, 75, SCREEN_WIDTH - 40, 75, MakeColor(45, 45, 61, 255));

            if (showFeedback) {
                if (lastAnswerCorrect) {
                    DrawText("CORRECT!", SCREEN_WIDTH / 2 - MeasureText("CORRECT!", 48) / 2, SCREEN_HEIGHT / 2 - 40, 48, MakeColor(0, 228, 48, 255));
                } else {
                    DrawText("WRONG!", SCREEN_WIDTH / 2 - MeasureText("WRONG!", 48) / 2, SCREEN_HEIGHT / 2 - 40, 48, MakeColor(190, 33, 61, 255));
                }
            } else {
                const char* textToDraw = GetColorName(currentWordIdx);
                Color inkColor = GetColorValue(currentColorIdx);
                
                int textWidth = MeasureText(textToDraw, 64);
                DrawText(textToDraw, SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - 40, 64, inkColor);
            }

            DrawText("Click the button that matches the INK COLOR!", SCREEN_WIDTH / 2 - MeasureText("Click the button that matches the INK COLOR!", 20) / 2, 380, 20, MakeColor(200, 200, 200, 255));

            for (int i = 0; i < COLOR_COUNT; i++) {
                buttons[i].Draw();
            }
        }

        EndDrawing();
    }
};

// ==========================================
// 3. MAIN APPLICATION ENTRY
// ==========================================
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Cognitive Stroop Test");
    SetTargetFPS(60);
    srand(time(NULL)); 

    StroopGame game;

    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    CloseWindow();
    return 0;
}