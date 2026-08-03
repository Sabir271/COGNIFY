#include "UIStyle.h"
#include <stdlib.h>
#include <time.h>

const int COLOR_COUNT = 5;

enum GameState {
    STATE_START,
    STATE_PLAYING,
    STATE_GAMEOVER
};

Color MakeColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

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

Color GetColorValue(int id) {
    switch (id) {
        case 0: return MakeColor(230, 41, 55, 255);   
        case 1: return MakeColor(0, 121, 242, 255);   
        case 2: return MakeColor(0, 228, 48, 255);    
        case 3: return MakeColor(253, 249, 0, 255);   
        case 4: return MakeColor(200, 122, 255, 255); 
        default: return MakeColor(0, 0, 0, 255);      
    }
}

class ColorButton {
private:
    Rectangle bounds;
    Color buttonColor;
    int colorId;

public:
    ColorButton() {
        bounds.x = 0.0f; bounds.y = 0.0f; bounds.width = 0.0f; bounds.height = 0.0f;
        buttonColor = MakeColor(0, 0, 0, 255); 
        colorId = -1;
    }

    void Setup(float x, float y, float width, float height, int id, Color color) {
        bounds.x = x; bounds.y = y; bounds.width = width; bounds.height = height;
        colorId = id; buttonColor = color;
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

    void UpdateButtonLayout(int sw, int sh) {
        float btnWidth = sw * 0.12f;
        if (btnWidth < 90) btnWidth = 90;
        if (btnWidth > 160) btnWidth = 160;
        float btnHeight = sh * 0.09f;
        if (btnHeight < 50) btnHeight = 50;
        if (btnHeight > 90) btnHeight = 90;
        float spacing = btnWidth * 0.18f;
        float totalWidth = (COLOR_COUNT * btnWidth) + ((COLOR_COUNT - 1) * spacing);
        float startX = (sw - totalWidth) / 2.0f;
        float btnY = sh * 0.72f;

        for (int i = 0; i < COLOR_COUNT; i++) {
            float posX = startX + i * (btnWidth + spacing);
            buttons[i].Setup(posX, btnY, btnWidth, btnHeight, i, GetColorValue(i));
        }
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
        score = 0; totalTrials = 0;
        showFeedback = false; lastAnswerCorrect = false;
        feedbackTimer = 0.0f; timeRemaining = 30.0f;

        UpdateButtonLayout(GetScreenWidth(), GetScreenHeight());
    }

    void StartNewSession() {
        score = 0; totalTrials = 0;
        showFeedback = false; lastAnswerCorrect = false;
        feedbackTimer = 0.0f; timeRemaining = 30.0f; 
        currentScene = STATE_PLAYING;
        NextTrial();
    }

    void Update() {
        switch (currentScene) {
            case STATE_START:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) StartNewSession();
                break;

            case STATE_PLAYING: {
                timeRemaining -= GetFrameTime();
                if (timeRemaining <= 0.0f) {
                    timeRemaining = 0.0f;
                    currentScene = STATE_GAMEOVER;
                    return;
                }

                if (showFeedback) {
                    feedbackTimer += GetFrameTime();
                    if (feedbackTimer >= 0.35f) { 
                        showFeedback = false; feedbackTimer = 0.0f;
                        NextTrial(); 
                    }
                    return; 
                }

                Vector2 mousePos = GetMousePosition();
                for (int i = 0; i < COLOR_COUNT; i++) {
                    if (buttons[i].IsClicked(mousePos)) {
                        totalTrials++; showFeedback = true;
                        if (buttons[i].GetColorId() == currentColorIdx) {
                            score++; lastAnswerCorrect = true;
                        } else {
                            lastAnswerCorrect = false;
                        }
                        break;
                    }
                }
                break;
            }

            case STATE_GAMEOVER:
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) StartNewSession();
                break;
        }
    }

    void Draw() {
        BeginDrawing();
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        UpdateButtonLayout(sw, sh);

        Color bgTop = {14, 22, 38, 255};
        Color bgBottom = {22, 32, 54, 255};
        DrawRectangleGradientV(0, 0, sw, sh, bgTop, bgBottom);
        DrawCircle((int)(sw*0.94f), (int)(sh*0.15f), (int)(sw*0.12f), {255, 255, 255, 10});
        DrawCircle((int)(sw*0.12f), (int)(sh*0.82f), (int)(sw*0.08f), {255, 255, 255, 12});

        float margin = sw * 0.05f;
        Rectangle header = { margin, sh * 0.03f, sw - margin * 2.0f, sh * 0.14f };
        DrawRectangleRounded(header, 0.30f, 18, {255, 255, 255, 14});
        DrawRectangleRoundedLines(header, 0.30f, 18, {255, 255, 255, 60});
        float headerCenteredY = header.y + header.height * 0.18f;
        DrawText("STROOP COGNITIVE TEST", sw / 2 - MeasureText("STROOP COGNITIVE TEST", 36) / 2, headerCenteredY, 36, Color{255, 245, 140, 255});
        DrawText("Match the ink color, not the word meaning.", sw / 2 - MeasureText("Match the ink color, not the word meaning.", 18) / 2, headerCenteredY + sh * 0.045f, 18, Color{210, 220, 240, 220});

        DrawText("Press ESC to Exit | F11 for Fullscreen", margin, header.y + header.height - sh * 0.02f, 12, MakeColor(140, 140, 160, 255));

        Rectangle mainCard = { sw * 0.08f, sh * 0.18f, sw * 0.84f, sh * 0.65f };
        DrawRectangleRounded(mainCard, 0.28f, 18, {18, 28, 48, 220});
        DrawRectangleRoundedLines(mainCard, 0.28f, 18, {255, 255, 255, 38});

        if (currentScene == STATE_START) {
            Rectangle rulesPanel = { mainCard.x + sw * 0.02f, mainCard.y + sh * 0.02f, mainCard.width - sw * 0.04f, mainCard.height - sh * 0.16f };
            DrawRectangleRounded(rulesPanel, 0.26f, 18, {16, 24, 42, 220});
            DrawRectangleRoundedLines(rulesPanel, 0.26f, 18, {255, 255, 255, 40});

            const char* rule1 = "1. Words will appear in random colors.";
            const char* rule2 = "2. Ignore what the text spells out completely!";
            const char* rule3 = "3. Click the bottom color button that matches the INK COLOR.";
            const char* rule4 = "4. Race against the 30 second countdown timer.";

            DrawText(rule1, rulesPanel.x + sw * 0.02f, rulesPanel.y + sh * 0.04f, 22, MakeColor(245, 245, 245, 255));
            DrawText(rule2, rulesPanel.x + sw * 0.02f, rulesPanel.y + sh * 0.10f, 22, MakeColor(230, 180, 255, 255));
            DrawText(rule3, rulesPanel.x + sw * 0.02f, rulesPanel.y + sh * 0.16f, 22, MakeColor(245, 245, 245, 255));
            DrawText(rule4, rulesPanel.x + sw * 0.02f, rulesPanel.y + sh * 0.22f, 22, MakeColor(245, 245, 245, 255));

            DrawText("Press [ENTER] or [SPACE] to Begin", sw / 2 - MeasureText("Press [ENTER] or [SPACE] to Begin", 24) / 2, rulesPanel.y + rulesPanel.height - sh * 0.05f, 24, MakeColor(130, 255, 180, 255));

        } else if (currentScene == STATE_GAMEOVER) {
            Rectangle resultPanel = { mainCard.x + sw * 0.02f, mainCard.y + sh * 0.02f, mainCard.width - sw * 0.04f, mainCard.height - sh * 0.04f };
            DrawRectangleRounded(resultPanel, 0.28f, 20, {28, 40, 66, 220});
            DrawRectangleRoundedLines(resultPanel, 0.28f, 20, {255, 255, 255, 60});

            DrawText("TEST COMPLETE", sw / 2 - MeasureText("TEST COMPLETE", 36) / 2, resultPanel.y + sh * 0.04f, 36, MakeColor(255, 225, 120, 255));
            DrawLine(resultPanel.x + sw * 0.03f, resultPanel.y + sh * 0.10f, resultPanel.x + resultPanel.width - sw * 0.03f, resultPanel.y + sh * 0.10f, MakeColor(80, 95, 130, 255));

            float finalAccuracy = (totalTrials > 0) ? ((float)score / totalTrials) * 100.0f : 0.0f;
            const char* finalScoreStr = TextFormat("Correct Answers: %d", score);
            const char* totalAttemptsStr = TextFormat("Total Attempts: %d", totalTrials);
            const char* accuracyStr = TextFormat("Final Accuracy: %.1f%%", finalAccuracy);

            DrawText(finalScoreStr, sw / 2 - MeasureText(finalScoreStr, 24) / 2, resultPanel.y + sh * 0.15f, 24, MakeColor(245, 245, 245, 255));
            DrawText(totalAttemptsStr, sw / 2 - MeasureText(totalAttemptsStr, 24) / 2, resultPanel.y + sh * 0.21f, 24, MakeColor(200, 200, 220, 255));
            DrawText(accuracyStr, sw / 2 - MeasureText(accuracyStr, 24) / 2, resultPanel.y + sh * 0.27f, 24, MakeColor(120, 255, 170, 255));

            const char* evaluationText = GetEvaluationGrade();
            DrawText(evaluationText, sw / 2 - MeasureText(evaluationText, 22) / 2, resultPanel.y + sh * 0.34f, 22, MakeColor(190, 145, 255, 255));

            DrawText("Press [SPACE] to Try Again", sw / 2 - MeasureText("Press [SPACE] to Try Again", 20) / 2, resultPanel.y + sh * 0.44f, 20, MakeColor(180, 190, 210, 255));

        } else if (currentScene == STATE_PLAYING) {
            Rectangle playPanel = { mainCard.x + sw * 0.02f, mainCard.y + sh * 0.02f, mainCard.width - sw * 0.04f, mainCard.height - sh * 0.12f };
            DrawRectangleRounded(playPanel, 0.26f, 18, {18, 28, 48, 220});
            DrawRectangleRoundedLines(playPanel, 0.26f, 18, {255, 255, 255, 40});
            // Title centered at top of play area
            DrawText("STROOP TEST", sw / 2 - MeasureText("STROOP TEST", 26) / 2, playPanel.y + sh * 0.02f, 26, MakeColor(180, 180, 200, 255));

            // Compact stat row (right side)
            const char* scoreText = TextFormat("Score: %d", score);
            DrawText(scoreText, playPanel.x + playPanel.width - MeasureText(scoreText, 20) - sw * 0.03f, playPanel.y + sh * 0.02f, 20, MakeColor(245, 245, 245, 255));

            float accuracy = (totalTrials > 0) ? ((float)score / totalTrials) * 100.0f : 0.0f;
            const char* accuracyText = TextFormat("Acc: %.0f%%", accuracy);
            DrawText(accuracyText, playPanel.x + playPanel.width - MeasureText(accuracyText, 18) - sw * 0.22f, playPanel.y + sh * 0.02f, 18, MakeColor(120, 255, 160, 255));

            const char* timerText = TextFormat("Time: %.1fs", timeRemaining);
            Color timerColor = (timeRemaining <= 5.0f) ? MakeColor(230, 41, 55, 255) : MakeColor(253, 249, 0, 255);
            DrawText(timerText, playPanel.x + playPanel.width - MeasureText(timerText, 20) - sw * 0.03f, playPanel.y + playPanel.height - sh * 0.06f, 20, timerColor);

            if (showFeedback) {
                if (lastAnswerCorrect) {
                    DrawText("CORRECT!", sw / 2 - MeasureText("CORRECT!", 48) / 2, playPanel.y + playPanel.height * 0.48f, 48, MakeColor(0, 228, 48, 255));
                } else {
                    DrawText("WRONG!", sw / 2 - MeasureText("WRONG!", 48) / 2, playPanel.y + playPanel.height * 0.48f, 48, MakeColor(190, 33, 61, 255));
                }
            } else {
                const char* textToDraw = GetColorName(currentWordIdx);
                Color inkColor = GetColorValue(currentColorIdx);
                int textWidth = MeasureText(textToDraw, 64);
                DrawText(textToDraw, sw / 2 - textWidth / 2, playPanel.y + playPanel.height * 0.48f, 64, inkColor);
            }

            DrawText("Click the button that matches the INK COLOR!", sw / 2 - MeasureText("Click the button that matches the INK COLOR!", 20) / 2, playPanel.y + playPanel.height - sh * 0.04f, 20, MakeColor(200, 200, 220, 255));

            for (int i = 0; i < COLOR_COUNT; i++) buttons[i].Draw();
        }

        EndDrawing();
    }
};

void RunStroopTest() {
    StroopGame game;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_F11)) ToggleAppFullscreen();
        game.Update();
        game.Draw();
    }
}