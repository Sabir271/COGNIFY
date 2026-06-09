#include "raylib.h"
#include <string.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

// A few sample phrases for the test
const char* phrases[] = {
    "The quick brown fox jumps over the lazy dog.",
    "Programming in C with raylib is fun and fast.",
    "Artificial intelligence is shaping the future of technology.",
    "Keep your wits about you and your fingers on the home row."
};

int main(void) {
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib - Typing Test");
    SetTargetFPS(60);

    // Game State Variables
    int phraseIndex = 1; // Change this to try different phrases
    const char* targetText = phrases[phraseIndex];
    int targetLength = strlen(targetText);

    char typedText[256] = "\0";
    int typedLength = 0;
    int errors = 0;

    double startTime = 0.0;
    double totalTime = 0.0;
    bool active = false;
    bool finished = false;

    double wpm = 0.0;
    double accuracy = 0.0;

    while (!WindowShouldClose()) {
        // --- Update Logic ---
        if (!finished) {
            // Get the key pressed this frame
            int key = GetCharPressed();

            // Start the timer on the very first keypress
            if (key > 0 && !active && typedLength == 0) {
                active = true;
                startTime = GetTime();
            }

            // Handle text input
            while (key > 0) {
                // Only allow printable characters and make sure we don't overflow
                if ((key >= 32) && (key <= 125) && (typedLength < targetLength)) {
                    typedText[typedLength] = (char)key;
                    
                    // Track errors if the typed character doesn't match the target
                    if (typedText[typedLength] != targetText[typedLength]) {
                        errors++;
                    }
                    
                    typedLength++;
                    typedText[typedLength] = '\0';
                }
                key = GetCharPressed(); // Check next key in buffer
            }

            // Handle Backspace
            if (IsKeyPressed(KEY_BACKSPACE) && typedLength > 0) {
                typedLength--;
                typedText[typedLength] = '\0';
                // Note: Simple error tracking doesn't decrement errors on backspace here,
                // but it keeps the logic straightforward for a basic test.
            }

            // Check if finished
            if (typedLength == targetLength) {
                active = false;
                finished = true;
                totalTime = GetTime() - startTime;

                // WPM calculation: (Standard word is 5 characters)
                // WPM = (all typed chars / 5) / time in minutes
                wpm = ((double)typedLength / 5.0) / (totalTime / 60.0);

                // Accuracy calculation
                int correctChars = typedLength - errors;
                if (correctChars < 0) correctChars = 0;
                accuracy = ((double)correctChars / (double)targetLength) * 100.0;
            }
        }

        // Restart logic
        if (finished && IsKeyPressed(KEY_ENTER)) {
            typedLength = 0;
            typedText[0] = '\0';
            errors = 0;
            startTime = 0.0;
            totalTime = 0.0;
            active = false;
            finished = false;
            // Optionally cycle to the next phrase
            phraseIndex = (phraseIndex + 1) % 4;
            targetText = phrases[phraseIndex];
            targetLength = strlen(targetText);
        }

        // --- Draw Logic ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Raylib Typing Test", SCREEN_WIDTH / 2 - MeasureText("Raylib Typing Test", 24) / 2, 40, 24, MAROON);
        DrawText("Type the sentence below:", 50, 110, 18, DARKGRAY);

        // 1. Draw the Target Phrase Guide
        DrawText(targetText, 50, 160, 20, LIGHTGRAY);

        // 2. Draw the User's Progress Character-by-Character
        int offsetX = 0;
        for (int i = 0; i < typedLength; i++) {
            char character[2] = { typedText[i], '\0' };
            Color color = (typedText[i] == targetText[i]) ? GREEN : RED;
            
            DrawText(character, 50 + offsetX, 160, 20, color);
            offsetX += MeasureText(character, 20);
        }

        // 3. Draw Live Cursor / Timer
        if (active) {
            double liveTime = GetTime() - startTime;
            DrawText(TextFormat("Time: %.1fs", liveTime), 50, 220, 16, GRAY);
        }

        // 4. Draw Results Screen
        if (finished) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, 0.85f));
            
            DrawText("Results", SCREEN_WIDTH / 2 - MeasureText("Results", 28) / 2, 120, 28, MAROON);
            DrawText(TextFormat("Speed: %.1f WPM", wpm), SCREEN_WIDTH / 2 - 80, 180, 22, DARKGRAY);
            DrawText(TextFormat("Accuracy: %.1f%%", accuracy), SCREEN_WIDTH / 2 - 80, 215, 22, DARKGRAY);
            DrawText(TextFormat("Time Taken: %.2fs", totalTime), SCREEN_WIDTH / 2 - 80, 250, 22, DARKGRAY);
            
            DrawText("Press [ENTER] to try the next phrase", SCREEN_WIDTH / 2 - MeasureText("Press [ENTER] to try the next phrase", 16) / 2, 330, 16, GRAY);
        } else {
            DrawText("Start typing to begin the timer...", 50, 380, 14, LIGHTGRAY);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}

