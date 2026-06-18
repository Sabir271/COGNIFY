#include "raylib.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstdio>

static const int   SW = 1000, SH = 600;
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
    "because","between","another","through","someone","problem","nothing","without","usually","already","station","present","machine","hundred","million","process",
    "support","evening","reading","special","message","natural","together","remember","sentence","question"
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
    int x = 50, y = 110, lw = 0;
    for (int i = 0; i < BUF && i < wb.index() + 60; i++) {
        const char* w = wb.get(i);
        int cur = wb.index();
        Color col;
        if (i < cur)       col = {100, 100, 100, 255};
        else if (i == cur) col = YELLOW;
        else               col = LIGHTGRAY;

        if (i == cur) {
            Color hi = {255, 255, 0, 40};
            DrawRectangle(x-2, y-2, MeasureText(w, 22)+4, 26, hi);
        }
        DrawText(w, x, y, 22, col);
        x  += MeasureText(w, 22) + 10;
        lw += MeasureText(w, 22) + 10;
        if (lw > SW - 100) { x = 50; y += 34; lw = 0; }
    }
}

    void drawInput() {
        DrawRectangleLines(50, 290, SW - 100, 55, WHITE);
        char p[64];
        sprintf(p, "Type: %s", wb.current());
        DrawText(p, 58, 268, 18, GRAY);
        bool ok = strncmp(input.text(), wb.current(), strlen(input.text())) == 0;
        DrawText(input.text(), 60, 303, 22, ok ? GREEN : RED);
        if ((int)(GetTime() * 2) % 2 == 0)
            DrawRectangle(60 + MeasureText(input.text(), 22), 302, 2, 24, WHITE);
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
        BeginDrawing();
        Color bg = {18, 18, 24, 255};
        ClearBackground(bg);

        // UI Anchor Tip
        DrawText("Press ESC to Exit to Main Menu | F11 for Fullscreen", 20, 15, 16, GRAY);

        if (state == WAIT) {
            const char* msg = "Press any key to start the 30-second test!";
            const char* sub = "SPACE = submit each word";
            DrawText(msg, SW/2 - MeasureText(msg,22)/2, SH/2-20, 22, WHITE);
            DrawText(sub, SW/2 - MeasureText(sub,18)/2, SH/2+20, 18, GRAY);
        }
        else if (state == PLAY) {
            char t[16];
            sprintf(t, "%.1f s", timer.timeLeft());
            Color tc = timer.timeLeft() < 5.f ? RED : WHITE;
            DrawText(t, SW/2 - MeasureText(t,36)/2, 30, 36, tc);
            drawParagraph();
            drawInput();
            char s[128];
            sprintf(s, "WPM: %.0f   Accuracy: %.0f%%   Correct Words: %d   Miskeys: %d",
                stats.wpm(timer.elapsed()), stats.accuracy(),
                stats.getCorrectWords(), stats.getWrongKeys());
            DrawText(s, 50, 390, 20, SKYBLUE);
            DrawText("SPACE = submit word", 50, 550, 16, DARKGRAY);
        }
        else {
            DrawText("Time's Up!", SW/2 - MeasureText("Time's Up!",48)/2, 80, 48, GOLD);
            char r[256];
            sprintf(r, "WPM:            %.1f\nAccuracy:       %.1f%%\nCorrect Words:  %d\nMiskeys:        %d",
                stats.wpm(timer.elapsed()), stats.accuracy(),
                stats.getCorrectWords(), stats.getWrongKeys());
            DrawText(r, SW/2 - 120, 200, 28, WHITE);
            DrawText("ENTER to restart",
                SW/2 - MeasureText("ENTER to restart",22)/2, 450, 22, YELLOW);
        }
        EndDrawing();
    }
};

// WRITE/EDIT YOUR CODES ABOVE THIS LINE ONLY DO NOT EDIT THIS BLOCK OF CODE!!!
void RunTypingTest() {
    // Dynamic resizing window trick to fit the text design gracefully
    SetWindowSize(1000, 600); 
    TypingTest game;
    
    while (!WindowShouldClose()) {
        // Look for the absolute exit flag
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
        
        game.update();
        game.draw();
    }
}