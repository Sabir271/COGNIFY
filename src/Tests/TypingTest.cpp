#include "UIStyle.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstdio>

static const int   START_SW = 1200, START_SH = 800;
static const float TEST_TIME = 30.0f;
static const int   POOL = 200, BUF = 400;

static const char* WORDS[POOL] = {
    // 1–2 letter
    "a","i","is","it","in","on","at","be","do","go",
    "he","me","my","no","of","or","so","to","up","us",
    "we","an","as","by","if","ok","hi","oh","am","any",

    // 3 letter
    "the","and","for","are","but","not","you","all","can","had",
    "her","was","one","our","out","day","get","has","him","his",
    "how","man","new","now","old","see","two","way","who","use",

    // 4 letter
    "that","this","with","they","from","word","have","just","know","time",
    "some","good","your","when","then","them","also","back","into","look",
    "come","here","make","most","over","such","well","work","very","will",

    // 5 letter
    "about","there","which","their","would","other","after","these","first","think",
    "could","great","every","right","place","small","found","still","where","those",
    "large","often","point","world","write","never","under","while","house","water",

    // 6 letter
    "people","should","always","before","around","things","better","little","really","almost",
    "school","mother","father","friend","summer","follow","simple","across","behind","change",
    "family","listen","number","system","toward","travel","strong","enough","finger","happen",

    // 7+ letter
    "because","between","another","through","someone","problem","nothing","without","example","kitchen",
    "quickly","quietly","outside","morning","usually","already","however","student","teacher","country",
    "brother","chapter","forward","general","history","several","thought","tonight","product","running",
    "station","present","machine","hundred","million","process","quality","support","evening","reading",
    "special","message","picture","natural","business","practice","together","remember","sentence","question"
};

// ── 1. WordBuffer ─────────────────────────────────────────────
class WordBuffer {
    char words[BUF][16];
    int  idx;
public:
    WordBuffer() { generate(); }
    void generate() {
        for (int i = 0; i < BUF; i++) strcpy(words[i], WORDS[rand() % POOL]);
        idx = 0;
    }
    void        advance()        { idx = (idx + 1) % BUF; }
    const char* current()  const { return words[idx]; }
    const char* get(int i) const { return words[i]; }
    int         index()    const { return idx; }
};

// ── 2. InputBox ───────────────────────────────────────────────
class InputBox {
    char buf[64];
    int  len;
public:
    bool backspacedEmpty;
public:
    InputBox() { clear(); }
    void clear() { memset(buf, 0, 64); len = 0; backspacedEmpty = false; }
    void setText(const char* s) {
        clear();
        for (int i = 0; s[i] && len < 62; i++) buf[len++] = s[i];
        buf[len] = '\0';
    }
    int update() {
        backspacedEmpty = false;
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (len > 0) buf[--len] = '\0';
            else backspacedEmpty = true;
            return 0;
        }
        int typed = 0;
        int c = GetCharPressed();
        if (c > 32 && c < 127 && len < 62) {
            buf[len++] = c; buf[len] = '\0';
            typed = c;
        }
        while (GetCharPressed()) {}
        return typed;
    }
    const char* text()        const { return buf; }
    bool        wantsGoBack() const { return backspacedEmpty; }
};

// ── 3. Stats ──────────────────────────────────────────────────
class Stats {
    int correctKeys, wrongKeys, correctWords;
public:
    Stats() { reset(); }
    void  reset()                   { correctKeys = wrongKeys = correctWords = 0; }
    void  recordKey(bool ok)        { ok ? correctKeys++ : wrongKeys++; }
    void  recordWord()              { correctWords++; }
    float wpm(float s)        const { return s > 0 ? correctWords / (s / 60.f) : 0.f; }
    float accuracy()          const {
        int t = correctKeys + wrongKeys;
        return t ? correctKeys * 100.f / t : 100.f;
    }
    int getCorrectWords()     const { return correctWords; }
    int getWrongKeys()        const { return wrongKeys; }
};

// ── 4. Timer ──────────────────────────────────────────────────
class Timer {
    float left;
    bool  on;
public:
    Timer() { reset(); }
    void  reset()          { left = TEST_TIME; on = false; }
    void  start()          { on = true; }
    void  update(float dt) { if (on && left > 0) left -= dt; }
    float timeLeft() const { return left < 0 ? 0.f : left; }
    float elapsed()  const { return TEST_TIME - timeLeft(); }
    bool  finished() const { return on && left <= 0; }
};

// ── 5. TypingTest (controller) ────────────────────────────────
class TypingTest {
    WordBuffer wb;
    InputBox   input;
    Stats      stats;
    Timer      timer;
    enum { WAIT, PLAY, DONE } state;

    void submit() {
        if (strcmp(input.text(), wb.current()) == 0) stats.recordWord();
        wb.advance();
        input.clear();
    }

  void drawParagraph() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int panelLeft = (int)(sw * 0.08f);
    int panelRight = (int)(sw * 0.92f);
    int maxWidth = panelRight - panelLeft;
    int x = panelLeft;
    int y = (int)(sh * 0.23f);
    int lineWidth = 0;

    for (int i = 0; i < BUF && i < wb.index() + 60; i++) {
        const char* w = wb.get(i);
        int cur = wb.index();
        Color col;
        if (i < cur)       col = {100, 100, 100, 255};
        else if (i == cur) col = YELLOW;
        else               col = LIGHTGRAY;

        int wordWidth = MeasureTextUI(w, 22);
        if (i == cur) {
            Color hi = {255, 255, 0, 40};
            DrawRectangle(x - 2, y - 2, wordWidth + 4, 26, hi);
        }

        DrawTextUI(w, x, y, 22, col);
        x += wordWidth + 10;
        lineWidth += wordWidth + 10;

        if (lineWidth > maxWidth) {
            x = panelLeft;
            y += 34;
            lineWidth = 0;
        }
    }
}

    void drawInput() {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        int boxX = (int)(sw * 0.08f);
        int boxY = (int)(sh * 0.58f);
        int boxW = (int)(sw * 0.84f);
        int boxH = (int)(sh * 0.085f);

        DrawRectangleRounded({(float)boxX, (float)boxY, (float)boxW, (float)boxH}, 0.20f, 16, Color{255, 255, 255, 18});
        DrawRectangleRoundedLines({(float)boxX, (float)boxY, (float)boxW, (float)boxH}, 0.20f, 16, Color{255, 255, 255, 110});
        char p[64];
        sprintf(p, "Type: %s", wb.current());
        DrawTextUI(p, boxX + 12, boxY - 26, 18, GRAY);
        bool ok = strncmp(input.text(), wb.current(), strlen(input.text())) == 0;
        DrawTextUI(input.text(), boxX + 16, boxY + (boxH / 2) - 10, 22, ok ? GREEN : RED);
        if ((int)(GetTime() * 2) % 2 == 0)
            DrawRectangle(boxX + 16 + MeasureTextUI(input.text(), 22), boxY + (boxH / 2) - 11, 2, 24, WHITE);
    }

public:
    TypingTest() { state = WAIT; }

    void update() {
        float dt = GetFrameTime();
        if (state == WAIT) {
            // Filter out ESC key so it doesn't accidentally trigger a game start when exiting
            int key = GetKeyPressed();
            if ((key != 0 && key != KEY_ESCAPE) || GetCharPressed()) {
                input.clear(); stats.reset(); wb.generate();
                timer.reset(); timer.start(); state = PLAY;
            }
        } else if (state == PLAY) {
            timer.update(dt);
            if (timer.finished()) { state = DONE; return; }

            int typed = input.update();
            if (typed != 0) {
                int pos = (int)strlen(input.text()) - 1;
                const char* target = wb.current();
                bool ok = (target[pos] != '\0' && typed == target[pos]);
                stats.recordKey(ok);
            }
            if (IsKeyPressed(KEY_SPACE)) submit();
        } else {
            if (IsKeyPressed(KEY_ENTER)) state = WAIT;
        }
    }

    void draw() {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        BeginDrawing();
        Color bgTop = {14, 18, 30, 255};
        Color bgBottom = {26, 32, 56, 255};
        DrawRectangleGradientV(0, 0, sw, sh, bgTop, bgBottom);
        DrawCircle(sw * 0.12f, sh * 0.16f, sw * 0.072f, {255, 255, 255, 12});
        DrawCircle(sw * 0.88f, sh * 0.86f, sw * 0.076f, {255, 255, 255, 10});

        Rectangle topPanel = { sw * 0.04f, sh * 0.03f, sw * 0.92f, sh * 0.14f };
        DrawPanel(topPanel, Color{255, 255, 255, 14}, Color{255, 255, 255, 85}, 0.30f, 18);
        DrawTextUI("TYPING SPEED CHALLENGE", sw * 0.06f, sh * 0.05f, 30, WHITE);
        DrawTextUI("Type accurately and keep the rhythm steady.", sw * 0.06f, sh * 0.09f, 18, Color{205, 215, 235, 220});

        char t[16];
        sprintf(t, "%.1f s", timer.timeLeft());
        Color tc = timer.timeLeft() < 5.f ? RED : WHITE;
        Rectangle timerPanel = { sw * 0.72f, sh * 0.045f, sw * 0.20f, sh * 0.08f };
        DrawRectangleRounded(timerPanel, 0.22f, 12, Color{32, 44, 72, 220});
        DrawRectangleRoundedLines(timerPanel, 0.22f, 12, Color{255, 255, 255, 100});
        DrawTextUI("TIME LEFT", timerPanel.x + 16, timerPanel.y + 10, 16, Color{190, 210, 230, 190});
        DrawTextUI(t, timerPanel.x + timerPanel.width * 0.5f - MeasureTextUI(t, 30) / 2, timerPanel.y + timerPanel.height * 0.35f, 30, tc);
        DrawTextUI("s", timerPanel.x + timerPanel.width - 26, timerPanel.y + timerPanel.height * 0.35f, 30, tc);

        DrawTextUI("Press ESC to Exit to Main Menu", sw * 0.06f, sh * 0.015f, 14, Color{170, 180, 200, 255});

        if (state == WAIT) {
            Rectangle prompt = { sw * 0.18f, sh * 0.22f, sw * 0.64f, sh * 0.16f };
            DrawPanel(prompt, Color{255, 255, 255, 20}, Color{255, 255, 255, 75}, 0.26f, 18);
            DrawTextUI("Press any key to start the 30-second test!", sw/2 - MeasureTextUI("Press any key to start the 30-second test!",22)/2, sh * 0.28f, 22, WHITE);
            DrawTextUI("SPACE = submit each word", sw/2 - MeasureTextUI("SPACE = submit each word",18)/2, sh * 0.33f, 18, Color{180, 190, 210, 255});
        }
        else if (state == PLAY) {
            Rectangle content = { sw * 0.06f, sh * 0.19f, sw * 0.88f, sh * 0.62f };
            DrawPanel(content, Color{255, 255, 255, 18}, Color{255, 255, 255, 75}, 0.26f, 18);
            drawParagraph();
            drawInput();

            Rectangle footer = { sw * 0.06f, sh * 0.82f, sw * 0.88f, sh * 0.11f };
            DrawPanel(footer, Color{255, 255, 255, 18}, Color{255, 255, 255, 95}, 0.24f, 16);
            char s[128];
            sprintf(s, "WPM: %.0f   Accuracy: %.0f%%   Correct: %d   Errors: %d",
                stats.wpm(timer.elapsed()), stats.accuracy(),
                stats.getCorrectWords(), stats.getWrongKeys());
            DrawTextUI(s, footer.x + 16, footer.y + 18, 20, SKYBLUE);
            DrawTextUI("SPACE = submit word", footer.x + footer.width - MeasureTextUI("SPACE = submit word", 16) - 16, footer.y + 20, 16, Color{200, 210, 230, 200});
        }
        else {
            Rectangle resultsPanel = { sw * 0.12f, sh * 0.18f, sw * 0.76f, sh * 0.40f };
            DrawPanel(resultsPanel, Color{255, 255, 255, 18}, Color{255, 255, 255, 90}, 0.28f, 20);
            DrawTextUI("Time's Up!", sw/2 - MeasureTextUI("Time's Up!",48)/2, sh * 0.22f, 48, GOLD);
            char r[256];
            sprintf(r, "WPM:            %.1f\nAccuracy:       %.1f%%\nCorrect Words:  %d\nMiskeys:        %d",
                stats.wpm(timer.elapsed()), stats.accuracy(),
                stats.getCorrectWords(), stats.getWrongKeys());
            DrawTextUI(r, sw/2 - MeasureTextUI(r, 28) / 2, sh * 0.31f, 28, WHITE);
            DrawTextUI("ENTER to restart", sw/2 - MeasureTextUI("ENTER to restart",22)/2, sh * 0.55f, 22, YELLOW);
        }
        EndDrawing();
    }
};

// WRITE/EDIT YOUR CODES ABOVE THIS LINE ONLY DO NOT EDIT THIS BLOCK OF CODE!!!
void RunTypingTest() {
    SetWindowSize(START_SW, START_SH);
    TypingTest game;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;

        game.update();
        game.draw();
    }
}