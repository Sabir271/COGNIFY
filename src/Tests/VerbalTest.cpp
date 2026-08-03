#include "UIStyle.h"
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace std;

class Word
{
public:
    string text;

    Word() {}

    Word(string t)
    {
        text = t;
    }
};

class Game
{
private:
    Word words[40] =
    {
        Word("apocalypse"), Word("cognitive"), Word("meticulous"), Word("ubiquitous"), Word("enigmatic"),
        Word("sophisticated"), Word("quarantine"), Word("philosophy"), Word("renaissance"), Word("magnificent"),
        Word("eccentric"), Word("symphony"), Word("labyrinth"), Word("hypothesis"), Word("consequence"),
        Word("trajectory"), Word("hierarchy"), Word("impeccable"), Word("revolution"), Word("ambiguous"),
        Word("spectrum"), Word("subsequent"), Word("exquisite"), Word("metamorphosis"), Word("paradoxical"),
        Word("phenomenon"), Word("resilience"), Word("illustration"), Word("synthesis"), Word("camaraderie"),
        Word("temperature"), Word("benevolent"), Word("approximate"), Word("adolescence"), Word("consolation"),
        Word("methodology"), Word("articulate"), Word("conspicuous"), Word("productivity"), Word("deteriorate")
    };

    string seenWords[50];

    int seenCount = 0;
    int currentWord = 0;
    int score = 0;
    int lives = 1;
    float timeLeft = 60.0f;
    bool gameOver = false;

    bool isSeenWord(const string& text) const {
        for (int i = 0; i < seenCount; i++) {
            if (seenWords[i] == text) return true;
        }
        return false;
    }

int chooseNextWordIndex() const
{
    int lastWord = currentWord;

    // 45% chance of showing a previously seen word
    if (seenCount > 0 && (rand() % 100) <55)
    {
        int attempts = 20;

        while (attempts--)
        {
            string target = seenWords[rand() % seenCount];

            for (int i = 0; i < 40; i++)
            {
                if (words[i].text == target && i != lastWord)
                    return i;
            }
        }
    }

    // Otherwise choose a random unseen word
    int unseen[40];
    int count = 0;

    for (int i = 0; i < 40; i++)
    {
        if (!isSeenWord(words[i].text) && i != lastWord)
        {
            unseen[count++] = i;
        }
    }

    if (count > 0)
        return unseen[rand() % count];

    // If every word has been seen, choose any word except the current one
    int next;
    do
    {
        next = rand() % 40;
    }
    while (next == lastWord);

    return next;
}

    void shuffleWords() {
        for (int i = 39; i > 0; --i) {
            int j = rand() % (i + 1);
            swap(words[i], words[j]);
        }
    }

public:
    Game() {
        shuffleWords();
        currentWord = chooseNextWordIndex();
    }

    void Update()
    {
        if (gameOver)
            return;

        timeLeft -= GetFrameTime();

        if (timeLeft <= 0)
        {
            timeLeft = 0;
            gameOver = true;
            return;
        }

        bool answerSeen = false;
        bool pressed = false;

        Vector2 mpos = GetMousePosition();
        Rectangle btnSeen = { GetScreenWidth() * 0.15f, GetScreenHeight() * 0.62f, GetScreenWidth() * 0.28f, GetScreenHeight() * 0.12f };
        Rectangle btnUnseen = { GetScreenWidth() * 0.57f, GetScreenHeight() * 0.62f, GetScreenWidth() * 0.28f, GetScreenHeight() * 0.12f };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mpos, btnSeen)) { answerSeen = true; pressed = true; }
            else if (CheckCollisionPointRec(mpos, btnUnseen)) { answerSeen = false; pressed = true; }
        }

        if (!pressed) return;

        bool actuallySeen = false;

        for (int i = 0; i < seenCount; i++)
        {
            if (seenWords[i] == words[currentWord].text)
            {
                actuallySeen = true;
                break;
            }
        }

        if (answerSeen == actuallySeen)
        {
            score++;
        }
        else
        {
            lives--;
            if (lives <= 0)
            {
                gameOver = true;
            }
        }

        if (seenCount < 50) {
            seenWords[seenCount++] = words[currentWord].text;
        }
        currentWord = chooseNextWordIndex();
    }

    void Draw()
    {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        Color bgTop = {11, 17, 35, 255};
        Color bgBottom = {40, 59, 96, 255};
        ClearBackground(bgTop);
        DrawRectangleGradientV(0, 0, sw, sh, bgTop, bgBottom);

        Rectangle outer = { sw * 0.05f, sh * 0.04f, sw * 0.90f, sh * 0.92f };
        DrawRectangleRounded(outer, 0.22f, 20, {255, 255, 255, 18});
        DrawRectangleRoundedLines(outer, 0.22f, 20, {255, 255, 255, 40});

        DrawText("Press ESC to Exit to Main Menu", sw * 0.08f, sh * 0.06f, 14, {192, 205, 225, 255});

        Rectangle infoBar = { sw * 0.08f, sh * 0.12f, sw * 0.84f, sh * 0.09f };
        DrawRectangleRounded(infoBar, 0.20f, 16, {255, 255, 255, 18});
        DrawText(TextFormat("Score: %i", score), sw * 0.10f, sh * 0.16f, 20, {122, 214, 255, 255});
        DrawText(TextFormat("Lives: %i", lives), sw * 0.34f, sh * 0.16f, 20, {255, 112, 112, 255});
        DrawText(TextFormat("Time: %.0f", timeLeft), sw * 0.55f, sh * 0.16f, 20, {255, 255, 255, 255});
        DrawText("Verbal Memory Challenge", sw * 0.72f, sh * 0.16f, 20, {245, 248, 255, 255});

        if (!gameOver)
        {
            Rectangle wordCard = { sw * 0.15f, sh * 0.24f, sw * 0.70f, sh * 0.22f };
            DrawRectangleRounded(wordCard, 0.24f, 20, {102, 130, 255, 40});
            DrawRectangleRoundedLines(wordCard, 0.24f, 20, {255, 255, 255, 70});
            DrawText(words[currentWord].text.c_str(), sw / 2 - MeasureText(words[currentWord].text.c_str(), 60) / 2, wordCard.y + wordCard.height * 0.35f, 60, WHITE);
            DrawText("Did you see this word earlier in the round?", sw * 0.19f, wordCard.y - sh * 0.02f, 20, {220, 230, 255, 255});

            Rectangle btnSeen = { sw * 0.15f, sh * 0.60f, sw * 0.28f, sh * 0.12f };
            Rectangle btnUnseen = { sw * 0.57f, sh * 0.60f, sw * 0.28f, sh * 0.12f };
            Vector2 mouse = GetMousePosition();
            bool overSeen = CheckCollisionPointRec(mouse, btnSeen);
            bool overUnseen = CheckCollisionPointRec(mouse, btnUnseen);

            Color seenColor = overSeen ? Color{76, 161, 255, 255} : Color{88, 132, 255, 255};
            Color unseenColor = overUnseen ? Color{218, 108, 178, 255} : Color{189, 88, 153, 255};
            Color cardBorder = {255, 255, 255, 120};
            DrawRectangleRounded(btnSeen, 0.24f, 16, seenColor);
            DrawRectangleRoundedLines(btnSeen, 0.24f, 16, cardBorder);
            DrawRectangleRounded(btnUnseen, 0.24f, 16, unseenColor);
            DrawRectangleRoundedLines(btnUnseen, 0.24f, 16, cardBorder);

            DrawText("SEEN", btnSeen.x + btnSeen.width * 0.25f, btnSeen.y + btnSeen.height * 0.25f, 28, WHITE);
            DrawText("UNSEEN", btnUnseen.x + btnUnseen.width * 0.22f, btnUnseen.y + btnUnseen.height * 0.25f, 28, WHITE);

            Rectangle hintPanel = { sw * 0.12f, btnSeen.y - sh * 0.04f, sw * 0.76f, sh * 0.06f };
            DrawRectangleRounded(hintPanel, 0.18f, 10, {255, 255, 255, 18});
            DrawText("Choose the best option before the timer runs out.", sw * 0.215f, hintPanel.y + sh * 0.008f, 18, {210, 220, 235, 210});
        }
        else
        {
            Rectangle overlay = { sw * 0.22f, sh * 0.22f, sw * 0.56f, sh * 0.32f };
            DrawRectangleRounded(overlay, 0.24f, 20, {255, 255, 255, 28});
            DrawRectangleRoundedLines(overlay, 0.24f, 20, {255, 255, 255, 70});
            DrawText("SESSION COMPLETE", sw * 0.37f, sh * 0.27f, 42, {255, 170, 80, 255});
            DrawText(TextFormat("Final Score: %i", score), sw * 0.38f, sh * 0.36f, 32, WHITE);
            DrawText("Press ESC to Exit", sw * 0.385f, sh * 0.42f, 24, {220, 230, 255, 255});
        }
    }
};

void RunVerbalTest()
{
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_F11)) ToggleAppFullscreen();

        game.Update();

        BeginDrawing();
        game.Draw();
        EndDrawing();
    }
}