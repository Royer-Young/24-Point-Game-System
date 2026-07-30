#pragma once
#include<vector>
#include<string>
#include<algorithm>
#include <graphics.h>
#include<cstdlib>
#include<ctime>
#include<cctype>
#include<cmath>
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include"Button.h"
#include"Card.h"
#include"Expression.h"

using namespace std;

// 场景枚举：表示当前应该显示哪个界面
enum class Scene {
    START,
    RULES,
    LEVELS,
    GAME,
    RESULT,
    ANSWER
};

class Game {
private:

    IMAGE bgStart;
    IMAGE bgGame;
    IMAGE bgResult;
    IMAGE bgLevel;
    IMAGE bgIntro;
    IMAGE bgAns;

    // ---------- 当前场景 ----------
    Scene currentScene = Scene::START;

    // ---------- 关卡数据 ----------
    int maxUnlockedLevel = 1;
    int currentLevel = 1;
    vector<vector<int>> levelCardNumbers;
    vector<wstring> levelSolutionExpr;
    vector<int> levelBestTimeSeconds;   // 每一关目前最快用时，-1表示还没通关过

    int levelScrollOffset = 0;
    const int LEVELS_VIEW_TOP = 100;
    const int LEVELS_VIEW_BOTTOM = 600;
    const int LEVELS_BAR_W = 380;        // 横条宽度缩窄，给右边的用时留出空间
    const int LEVELS_BAR_H = 80;
    const int LEVELS_GAP = 15;
    const int LEVELS_SCROLLBAR_X = 760;  // 滚动条也跟着往右挪

    // ---------- 游戏核心数据 ----------
    vector<Card> cards;
    vector<int> cardNumbers;
    wstring currentSolutionExpr;
    vector<Button> opButtons;
    vector<char> opSymbols;
    Button submitButton;
    Button undoButton;

    // 表达式用"token序列 + 光标位置"表示，而不是简单的字符串追加
    struct Token {
        char type;      // 'N'(数字) 或 '+' '-' '*' '/' '(' ')'
        int cardIndex;  // type=='N'时对应第几张牌，否则-1
        string text;    // 显示文字，比如"11"或"+"
    };
    vector<Token> tokens;
    int cursorPos = 0;   // 插入点：在tokens[cursorPos]之前

    wstring statusMessage;

    // ---------- 计时 / 连胜 ----------
    clock_t gameStartClock = 0;
    double lastGameTimeSeconds = 0;
    int currentStreak = 0;

    // ---------- 开始界面 ----------
    Button startButton;
    Button rulesButton;

    // ---------- 玩法说明界面 ----------
    Button backFromRulesButton;

    // ---------- 选关界面 ----------
    Button backFromLevelsButton;

    // ---------- 结算画面 ----------
    Button continueButton;
    Button resultRestartButton;
    Button hintButton;
    Button homeButton;
    bool lastGameSuccess = false;

    // ---------- 提示答案界面 ----------
    Button backFromAnswerButton;

    // ---------- 背景音乐占位路径 ----------
    const wstring MUSIC_START = L"music\\startMusic.mp3";
    const wstring MUSIC_GAME = L"music\\gameMusic.mp3";
    const wstring MUSIC_WIN = L"music\\winMusic.mp3";
    const wstring MUSIC_LOSE = L"music\\loseMusic.mp3";

    // ---------- 游戏界面布局参数（放一起方便统一调整） ----------
    const int CARD_Y = 70;
    const int CARD_XS[4] = { 102, 257, 412, 567 };
    const int EXPR_BOX_X = 150, EXPR_BOX_Y = 280, EXPR_BOX_W = 500, EXPR_BOX_H = 70;
    const int OP_ROW1_Y = 370, OP_ROW2_Y = 455;
    const int SUBMIT_UNDO_Y = 545;
    const int STATUS_Y = 615;

public:
    Game() {}

    void init() {
        srand(time(0));

        loadimage(&bgStart, L"bgImage\\start.png", 800, 700);
        loadimage(&bgGame, L"bgImage\\game.png", 800, 700);
        loadimage(&bgResult, L"bgImage\\result.png", 800, 700);
        loadimage(&bgLevel, L"bgImage\\level.png", 800, 700);
        loadimage(&bgIntro, L"bgImage\\intro.png", 800, 700);
        loadimage(&bgAns, L"bgImage\\Ans.png", 800, 700);

        startButton = Button(290, 450, 220, 70, L"进入游戏");
        rulesButton = Button(290, 540, 220, 70, L"玩法说明");

        backFromRulesButton = Button(300, 600, 200, 60, L"返回");

        backFromLevelsButton = Button(300, 630, 200, 50, L"返回");

        opButtons.push_back(Button(215, OP_ROW1_Y, 110, 70, L"+"));
        opButtons.push_back(Button(345, OP_ROW1_Y, 110, 70, L"-"));
        opButtons.push_back(Button(475, OP_ROW1_Y, 110, 70, L"*"));
        opButtons.push_back(Button(215, OP_ROW2_Y, 110, 70, L"/"));
        opButtons.push_back(Button(345, OP_ROW2_Y, 110, 70, L"("));
        opButtons.push_back(Button(475, OP_ROW2_Y, 110, 70, L")"));

        opSymbols.push_back('+');
        opSymbols.push_back('-');
        opSymbols.push_back('*');
        opSymbols.push_back('/');
        opSymbols.push_back('(');
        opSymbols.push_back(')');

        submitButton = Button(220, SUBMIT_UNDO_Y, 150, 60, L"提交");
        undoButton = Button(430, SUBMIT_UNDO_Y, 150, 60, L"撤回");

        continueButton = Button(0, 0, 150, 60, L"下一关");
        resultRestartButton = Button(0, 0, 150, 60, L"重新挑战");
        hintButton = Button(0, 0, 150, 60, L"参考答案");
        homeButton = Button(0, 0, 150, 60, L"返回菜单");

        backFromAnswerButton = Button(300, 550, 200, 60, L"返回");

        playMusic(MUSIC_START);
    }

    void handleClick(int mx, int my) {
        switch (currentScene) {
        case Scene::START:  handleStartClick(mx, my);  break;
        case Scene::RULES:  handleRulesClick(mx, my);  break;
        case Scene::LEVELS: handleLevelsClick(mx, my); break;
        case Scene::GAME:   handleGameClick(mx, my);   break;
        case Scene::RESULT: handleResultClick(mx, my); break;
        case Scene::ANSWER: handleAnswerClick(mx, my); break;
        }
    }

    void draw() {
        switch (currentScene) {
        case Scene::START:  drawStartScene();  break;
        case Scene::RULES:  drawRulesScene();  break;
        case Scene::LEVELS: drawLevelsScene(); break;
        case Scene::GAME:   drawGameScene();   break;
        case Scene::RESULT: drawResultScene(); break;
        case Scene::ANSWER: drawAnswerScene(); break;
        }
    }

    void run() {
        initgraph(800, 700);
        init();

        BeginBatchDraw();

        ExMessage msg;
        while (true) {
            while (peekmessage(&msg, EM_MOUSE)) {
                if (msg.message == WM_LBUTTONDOWN) {
                    handleClick(msg.x, msg.y);
                }
                if (msg.message == WM_MOUSEWHEEL) {
                    handleScroll(msg.wheel);
                }
            }

            draw();
            FlushBatchDraw();

            Sleep(16);
        }

        EndBatchDraw();
        closegraph();
    }

private:

    // ============ 背景音乐 ============

    void playMusic(const wstring& filePath) {
        mciSendString(L"close bgmusic", NULL, 0, NULL);
        wstring cmd = L"open \"" + filePath + L"\" alias bgmusic";
        if (mciSendString(cmd.c_str(), NULL, 0, NULL) == 0) {
            mciSendString(L"play bgmusic repeat", NULL, 0, NULL);
        }
    }

    // ============ 开始界面 ============

    void handleStartClick(int mx, int my) {
        if (startButton.isClicked(mx, my)) {
            levelScrollOffset = 0;
            currentScene = Scene::LEVELS;
        }
        if (rulesButton.isClicked(mx, my)) {
            currentScene = Scene::RULES;
        }
    }

    void drawStartScene() {
        putimage(0, 0, &bgStart);

        setbkmode(TRANSPARENT);
        settextcolor(RGB(180, 60, 20));
        settextstyle(80, 0, L"华文行楷");
        wstring title = L"24点大挑战";
        int tx = (800 - textwidth(title.c_str())) / 2;
        outtextxy(tx, 150, title.c_str());

        startButton.draw();
        rulesButton.draw();
    }

    // ============ 玩法说明界面 ============

    void handleRulesClick(int mx, int my) {
        if (backFromRulesButton.isClicked(mx, my)) {
            currentScene = Scene::START;
        }
    }

    void drawRulesScene() {
        putimage(0, 0, &bgIntro);

        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        settextstyle(60, 0, L"华文行楷");
        outtextxy(100, 80, L"玩法说明");

        settextstyle(22, 0, L"华文行楷");
        outtextxy(100, 140, L"1. 点击四张卡牌和运算符按钮，插入到表达式框中光标所在的位置");
        outtextxy(100, 175, L"2. 点击表达式框内部，可以把光标移动到你点的那个位置，方便插入");
        outtextxy(100, 210, L"3. 每张卡牌只能使用一次，全部用完才能提交");
        outtextxy(100, 245, L"4. 算式结果等于24即可过关");
        outtextxy(100, 280, L"5. 点击撤销会删除光标左边紧挨着的那一个数字或符号");
        outtextxy(100, 315, L"6. 提交时会检查算式是否合理（比如括号有没有闭合），不合理会有提示");
        outtextxy(100, 350, L"7. 每一局保证一定有解，实在想不出可以在结算画面点提示");
        outtextxy(100, 385, L"8. 通关后会解锁下一关，选关界面可以随时重玩已通关的关卡");

        backFromRulesButton.draw();
    }

    // ============ 选关界面 ============

    void handleLevelsClick(int mx, int my) {
        int barX = (800 - LEVELS_BAR_W) / 2;

        for (int i = 1; i <= maxUnlockedLevel; i++) {
            int y = LEVELS_VIEW_TOP + (i - 1) * (LEVELS_BAR_H + LEVELS_GAP) - levelScrollOffset;
            if (y + LEVELS_BAR_H < LEVELS_VIEW_TOP || y > LEVELS_VIEW_BOTTOM) continue;

            Button levelBar(barX, y, LEVELS_BAR_W, LEVELS_BAR_H, L"");
            if (levelBar.isClicked(mx, my)) {
                startLevel(i);
                return;
            }
        }

        if (backFromLevelsButton.isClicked(mx, my)) {
            currentScene = Scene::START;
        }
    }

    void drawLevelsScene() {
        putimage(0, 0, &bgLevel);

        setbkmode(TRANSPARENT);

        // 最上面：最多连闯记录
        settextcolor(RGB(250, 0, 0));
        settextstyle(23, 0, L"华文行楷");
        wstring recordText = L"最多连闯 " + to_wstring(maxUnlockedLevel - 1) + L" 关";
        int rx = (800 - textwidth(recordText.c_str())) / 2;
        outtextxy(rx, 10, recordText.c_str());

        settextcolor(BLACK);
        settextstyle(31, 0, L"华文行楷");
        wstring title = L"选择关卡";
        int tx = (800 - textwidth(title.c_str())) / 2;
        outtextxy(tx, 40, title.c_str());

        int barX = (800 - LEVELS_BAR_W) / 2;

        for (int i = 1; i <= maxUnlockedLevel; i++) {
            int y = LEVELS_VIEW_TOP + (i - 1) * (LEVELS_BAR_H + LEVELS_GAP) - levelScrollOffset;
            if (y + LEVELS_BAR_H < LEVELS_VIEW_TOP || y > LEVELS_VIEW_BOTTOM) continue;

            wstring label = L"第" + to_wstring(i) + L"关";
            Button levelBar(barX, y, LEVELS_BAR_W, LEVELS_BAR_H, label);
            levelBar.draw();

            // 横条右边：本关最快用时（还没通关过就不显示）
            int idx = i - 1;
            if (idx < (int)levelBestTimeSeconds.size() && levelBestTimeSeconds[idx] >= 0) {
                wstring timeLabel = L"本关用时" + to_wstring(levelBestTimeSeconds[idx]) + L"秒";
                setbkmode(TRANSPARENT);
                settextcolor(BLACK);
                settextstyle(20, 0, L"华文行楷");
                int labelX = barX + LEVELS_BAR_W + 15;
                int labelY = y + (LEVELS_BAR_H - textheight(timeLabel.c_str())) / 2;
                outtextxy(labelX, labelY, timeLabel.c_str());
            }
        }

        drawLevelsScrollbar();
        backFromLevelsButton.draw();
    }

    void drawLevelsScrollbar() {
        int trackX = LEVELS_SCROLLBAR_X, trackY = LEVELS_VIEW_TOP, trackW = 12;
        int trackH = LEVELS_VIEW_BOTTOM - LEVELS_VIEW_TOP;

        setfillcolor(RGB(200, 200, 200));
        fillrectangle(trackX, trackY, trackX + trackW, trackY + trackH);

        int totalContentHeight = maxUnlockedLevel * (LEVELS_BAR_H + LEVELS_GAP);
        if (totalContentHeight <= trackH) return;

        int thumbH = trackH * trackH / totalContentHeight;
        if (thumbH < 20) thumbH = 20;

        int maxScroll = totalContentHeight - trackH;
        int thumbY = trackY + (levelScrollOffset * (trackH - thumbH)) / max(1, maxScroll);

        setfillcolor(RGB(120, 120, 160));
        fillrectangle(trackX, thumbY, trackX + trackW, thumbY + thumbH);
    }

    void handleScroll(int wheelDelta) {
        if (currentScene != Scene::LEVELS) return;

        levelScrollOffset -= wheelDelta / 120 * 40;

        int totalContentHeight = maxUnlockedLevel * (LEVELS_BAR_H + LEVELS_GAP);
        int viewHeight = LEVELS_VIEW_BOTTOM - LEVELS_VIEW_TOP;
        int maxScroll = totalContentHeight - viewHeight;
        if (maxScroll < 0) maxScroll = 0;

        if (levelScrollOffset < 0) levelScrollOffset = 0;
        if (levelScrollOffset > maxScroll) levelScrollOffset = maxScroll;
    }

    // ============ 游戏界面 ============

    void handleGameClick(int mx, int my) {
        // 先判断有没有点在表达式框内部——点里面是移动光标
        if (mx >= EXPR_BOX_X && mx <= EXPR_BOX_X + EXPR_BOX_W &&
            my >= EXPR_BOX_Y && my <= EXPR_BOX_Y + EXPR_BOX_H) {
            moveCursorByClick(mx);
            return;
        }

        for (int i = 0; i < cards.size(); i++) {
            if (cards[i].isClicked(mx, my)) {
                onCardClicked(i);
                return;
            }
        }

        for (int i = 0; i < opButtons.size(); i++) {
            if (opButtons[i].isClicked(mx, my)) {
                onOperatorClicked(opSymbols[i]);
                return;
            }
        }

        if (submitButton.isClicked(mx, my)) {
            onSubmit();
            return;
        }
        if (undoButton.isClicked(mx, my)) {
            onUndo();
            return;
        }
    }

    void drawGameScene() {
        putimage(0, 0, &bgGame);

        // 卡牌：Card类自己画占位方块，后期在Card::draw()里换图即可
        for (int i = 0; i < cards.size(); i++) {
            cards[i].draw();
        }
        for (int i = 0; i < opButtons.size(); i++) {
            opButtons[i].draw();
        }
        submitButton.draw();
        undoButton.draw();

        drawExpressionBox();
        drawStatusMessage();

        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        settextstyle(28, 0, L"华文行楷");
        wstring levelText = L"第" + to_wstring(currentLevel) + L"关";
        int lx = (800 - textwidth(levelText.c_str())) / 2;
        outtextxy(lx, 20, levelText.c_str());

        drawTimerBox();
    }

    void drawTimerBox() {
        int boxX = 15, boxY = 15, boxW = 150, boxH = 40;

        setfillcolor(WHITE);
        fillrectangle(boxX, boxY, boxX + boxW, boxY + boxH);
        setlinecolor(BLACK);
        rectangle(boxX, boxY, boxX + boxW, boxY + boxH);

        int elapsedSec = (int)((clock() - gameStartClock) / (double)CLOCKS_PER_SEC);
        wstring timeText = L"用时: " + to_wstring(elapsedSec) + L"秒";

        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        settextstyle(18, 0, L"华文行楷");
        int tx = boxX + (boxW - textwidth(timeText.c_str())) / 2;
        int ty = boxY + (boxH - textheight(timeText.c_str())) / 2;
        outtextxy(tx, ty, timeText.c_str());
    }

    // ============ 结算画面 ============

    void handleResultClick(int mx, int my) {
        if (lastGameSuccess && continueButton.isClicked(mx, my)) {
            startLevel(currentLevel + 1);
            return;
        }
        if (resultRestartButton.isClicked(mx, my)) {
            retryCurrentLevel();
            return;
        }
        if (hintButton.isClicked(mx, my)) {
            currentScene = Scene::ANSWER;
            return;
        }
        if (homeButton.isClicked(mx, my)) {
            currentScene = Scene::START;
            playMusic(MUSIC_START);
            return;
        }
    }

    void drawResultScene() {
        putimage(0, 0, &bgResult);

        int sec = (int)lastGameTimeSeconds;
        wstring timeText = L"本局用时: " + to_wstring(sec) + L"秒";
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        settextstyle(20, 0, L"华文行楷");
        outtextxy(20, 20, timeText.c_str());

        settextstyle(60, 0, L"华文行楷");
        wstring resultText = lastGameSuccess ? L"恭喜，成功了！" : L"很遗憾，未能凑成24";
        settextcolor(lastGameSuccess ? RGB(0, 150, 0) : RGB(200, 30, 30));
        int tx = (800 - textwidth(resultText.c_str())) / 2;
        outtextxy(tx, 250, resultText.c_str());

        wstring streakText = L"连续答对：" + to_wstring(currentStreak) + L"局";
        settextstyle(27, 0, L"华文行楷");
        settextcolor(RGB(80, 80, 160));
        int sx = (800 - textwidth(streakText.c_str())) / 2;
        outtextxy(sx, 370, streakText.c_str());

        if (lastGameSuccess) {
            continueButton.draw();
        }
        resultRestartButton.draw();
        hintButton.draw();
        homeButton.draw();
    }

    void layoutResultButtons() {
        int barW = 150, gap = 20, y = 560;

        if (lastGameSuccess) {
            int totalW = 4 * barW + 3 * gap;
            int startX = (800 - totalW) / 2;
            continueButton.setPosition(startX, y);
            resultRestartButton.setPosition(startX + (barW + gap), y);
            hintButton.setPosition(startX + 2 * (barW + gap), y);
            homeButton.setPosition(startX + 3 * (barW + gap), y);
        }
        else {
            int totalW = 3 * barW + 2 * gap;
            int startX = (800 - totalW) / 2;
            resultRestartButton.setPosition(startX, y);
            hintButton.setPosition(startX + (barW + gap), y);
            homeButton.setPosition(startX + 2 * (barW + gap), y);
        }
    }

    // ============ 提示答案界面 ============

    void handleAnswerClick(int mx, int my) {
        if (backFromAnswerButton.isClicked(mx, my)) {
            currentScene = Scene::RESULT;
        }
    }

    void drawAnswerScene() {
        putimage(0, 0, &bgAns);

        setbkmode(TRANSPARENT);
        settextcolor(RGB(60, 60, 60));
        settextstyle(35, 0, L"华文行楷");
        wstring title = L"参考答案";
        int titleX = (800 - textwidth(title.c_str())) / 2;
        outtextxy(titleX, 200, title.c_str());

        settextcolor(RGB(30, 100, 180));
        settextstyle(45, 0, L"华文行楷");
        int exprX = (800 - textwidth(currentSolutionExpr.c_str())) / 2;
        outtextxy(exprX, 280, currentSolutionExpr.c_str());

        backFromAnswerButton.draw();
    }

    // ============ 表达式：token + 光标 ============

    // a是左边紧邻token的类型(或'S'表示序列开头)，b是右边真实存在的token类型
    // 判断a紧挨着b是否是合法的相邻组合
    bool isValidPair(char a, char b) const {
        if (a == 'S') {
            return (b == 'N' || b == '(');   // 开头只能是数字或左括号
        }
        bool aIsValueEnd = (a == 'N' || a == ')');
        if (aIsValueEnd) {
            // 一个值刚结束，后面必须是运算符或右括号
            return (b == '+' || b == '-' || b == '*' || b == '/' || b == ')');
        }
        // a是'('或运算符，等待一个新值开始
        return (b == 'N' || b == '(');
    }

    // 判断在光标处插入类型为mid的token是否合法
    bool canInsertAt(char mid) const {
        char left = (cursorPos == 0) ? 'S' : tokens[cursorPos - 1].type;
        if (!isValidPair(left, mid)) return false;

        if (cursorPos < (int)tokens.size()) {
            char right = tokens[cursorPos].type;
            if (!isValidPair(mid, right)) return false;
        }

        if (mid == ')') {
            // 右括号还需要左边有一个尚未配对的左括号
            int openCount = 0;
            for (int i = 0; i < cursorPos; i++) {
                if (tokens[i].type == '(') openCount++;
                if (tokens[i].type == ')') openCount--;
            }
            if (openCount <= 0) return false;
        }

        return true;
    }

    string buildExpressionString() const {
        string s;
        for (int i = 0; i < (int)tokens.size(); i++) s += tokens[i].text;
        return s;
    }

    void moveCursorByClick(int mx) {
        settextstyle(34, 0, L"华文行楷");   // 必须和drawExpressionBox里用的字体/字号一致，textwidth才准

        int curX = EXPR_BOX_X + 15;
        int bestPos = 0;
        int bestDist = abs(mx - curX);

        for (int i = 0; i < (int)tokens.size(); i++) {
            wstring t = stringToWstring(tokens[i].text);
            curX += textwidth(t.c_str());
            int dist = abs(mx - curX);
            if (dist < bestDist) {
                bestDist = dist;
                bestPos = i + 1;
            }
        }
        cursorPos = bestPos;
    }

    // ============ 游戏逻辑 ============

    void onCardClicked(int index) {
        statusMessage = L"";

        if (cards[index].getUsed()) return;
        if (!canInsertAt('N')) return;

        Token t;
        t.type = 'N';
        t.cardIndex = index;
        t.text = to_string(cards[index].getNumber());
        tokens.insert(tokens.begin() + cursorPos, t);
        cursorPos++;

        cards[index].setUsed(true);
    }

    void onOperatorClicked(char op) {
        statusMessage = L"";

        if (!canInsertAt(op)) return;

        Token t;
        t.type = op;
        t.cardIndex = -1;
        t.text = string(1, op);
        tokens.insert(tokens.begin() + cursorPos, t);
        cursorPos++;
    }

    // 删除光标左边紧挨着的那一个token（数字或符号）
    void onUndo() {
        statusMessage = L"";

        if (cursorPos == 0) return;

        Token removed = tokens[cursorPos - 1];
        tokens.erase(tokens.begin() + cursorPos - 1);
        cursorPos--;

        if (removed.type == 'N') {
            cards[removed.cardIndex].setUsed(false);
        }
    }

    void onSubmit() {
        if (!allCardsUsed()) {
            statusMessage = L"还有牌没被使用";
            return;
        }

        string exprStr = buildExpressionString();
        Expression tempExpr;
        tempExpr.setExpressionString(exprStr);

        double result;
        bool valid = tempExpr.evaluate(result);

        if (!valid) {
            statusMessage = L"算式符号不合理，请检查一遍";
            return;   // 不合法，留在游戏界面，不进结算画面
        }

        lastGameSuccess = (fabs(result - 24.0) < 1e-6);
        lastGameTimeSeconds = (clock() - gameStartClock) / (double)CLOCKS_PER_SEC;

        if (lastGameSuccess) {
            currentStreak++;
            if (currentLevel + 1 > maxUnlockedLevel) {
                maxUnlockedLevel = currentLevel + 1;
            }

            int idx = currentLevel - 1;
            int sec = (int)lastGameTimeSeconds;
            if (levelBestTimeSeconds[idx] < 0 || sec < levelBestTimeSeconds[idx]) {
                levelBestTimeSeconds[idx] = sec;   // 只记录最快的一次
            }
        }
        else {
            currentStreak = 0;
        }

        layoutResultButtons();
        playMusic(lastGameSuccess ? MUSIC_WIN : MUSIC_LOSE);
        currentScene = Scene::RESULT;
    }

    void rebuildCardsFromNumbers() {
        cards.clear();
        for (int i = 0; i < 4; i++) {
            cards.push_back(Card(CARD_XS[i], CARD_Y, cardNumbers[i]));
        }
    }

    void ensureLevelGenerated(int level) {
        while ((int)levelCardNumbers.size() < level) {
            vector<int> nums(4);
            string solution;

            while (true) {
                for (int i = 0; i < 4; i++) nums[i] = randomNumber();
                if (trySolve24(nums, solution)) break;
            }

            levelCardNumbers.push_back(nums);
            levelSolutionExpr.push_back(stringToWstring(solution));
            levelBestTimeSeconds.push_back(-1);   // -1表示这一关还没有通关记录
        }
    }

    void startLevel(int level) {
        ensureLevelGenerated(level);

        currentLevel = level;
        cardNumbers = levelCardNumbers[level - 1];
        currentSolutionExpr = levelSolutionExpr[level - 1];

        rebuildCardsFromNumbers();
        tokens.clear();
        cursorPos = 0;
        statusMessage = L"";
        gameStartClock = clock();

        playMusic(MUSIC_GAME);
        currentScene = Scene::GAME;
    }

    void retryCurrentLevel() {
        rebuildCardsFromNumbers();
        tokens.clear();
        cursorPos = 0;
        statusMessage = L"";
        gameStartClock = clock();

        playMusic(MUSIC_GAME);
        currentScene = Scene::GAME;
    }

    bool allCardsUsed() const {
        for (int i = 0; i < cards.size(); i++) {
            if (!cards[i].getUsed()) return false;
        }
        return true;
    }

    int randomNumber() const {
        return rand() % 10 + 1;
    }

    // ---------- 保证有解 ----------

    double applyOp(double x, char op, double y, bool& ok) const {
        ok = true;
        if (op == '+') return x + y;
        if (op == '-') return x - y;
        if (op == '*') return x * y;
        if (op == '/') {
            if (fabs(y) < 1e-9) { ok = false; return 0; }
            return x / y;
        }
        ok = false;
        return 0;
    }

    bool trySolve24(const vector<int>& nums, string& outExpr) const {
        vector<int> order = { 0, 1, 2, 3 };
        sort(order.begin(), order.end());

        const char ops[4] = { '+', '-', '*', '/' };

        do {
            double a = nums[order[0]], b = nums[order[1]];
            double c = nums[order[2]], d = nums[order[3]];
            string sa = to_string(nums[order[0]]), sb = to_string(nums[order[1]]);
            string sc = to_string(nums[order[2]]), sd = to_string(nums[order[3]]);

            for (char op1 : ops) {
                for (char op2 : ops) {
                    for (char op3 : ops) {
                        bool ok1, ok2, ok3;

                        double r1 = applyOp(a, op1, b, ok1);
                        if (ok1) {
                            double r2 = applyOp(r1, op2, c, ok2);
                            if (ok2) {
                                double r3 = applyOp(r2, op3, d, ok3);
                                if (ok3 && fabs(r3 - 24.0) < 1e-6) {
                                    outExpr = "((" + sa + op1 + sb + ")" + op2 + sc + ")" + op3 + sd;
                                    return true;
                                }
                            }
                        }

                        double rbc = applyOp(b, op2, c, ok2);
                        if (ok2) {
                            double rA = applyOp(a, op1, rbc, ok1);
                            if (ok1) {
                                double r3 = applyOp(rA, op3, d, ok3);
                                if (ok3 && fabs(r3 - 24.0) < 1e-6) {
                                    outExpr = "(" + sa + op1 + "(" + sb + op2 + sc + "))" + op3 + sd;
                                    return true;
                                }
                            }
                        }

                        double rab = applyOp(a, op1, b, ok1);
                        double rcd = applyOp(c, op3, d, ok3);
                        if (ok1 && ok3) {
                            double r2 = applyOp(rab, op2, rcd, ok2);
                            if (ok2 && fabs(r2 - 24.0) < 1e-6) {
                                outExpr = "(" + sa + op1 + sb + ")" + op2 + "(" + sc + op3 + sd + ")";
                                return true;
                            }
                        }

                        double rbc2 = applyOp(b, op2, c, ok2);
                        if (ok2) {
                            double r3 = applyOp(rbc2, op3, d, ok3);
                            if (ok3) {
                                double r1b = applyOp(a, op1, r3, ok1);
                                if (ok1 && fabs(r1b - 24.0) < 1e-6) {
                                    outExpr = sa + op1 + "((" + sb + op2 + sc + ")" + op3 + sd + ")";
                                    return true;
                                }
                            }
                        }

                        double rcd2 = applyOp(c, op3, d, ok3);
                        if (ok3) {
                            double r2c = applyOp(b, op2, rcd2, ok2);
                            if (ok2) {
                                double r1c = applyOp(a, op1, r2c, ok1);
                                if (ok1 && fabs(r1c - 24.0) < 1e-6) {
                                    outExpr = sa + op1 + "(" + sb + op2 + "(" + sc + op3 + sd + "))";
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        } while (next_permutation(order.begin(), order.end()));

        return false;
    }

    // ---------- 绘制辅助 ----------

    wstring stringToWstring(const string& s) const {
        wstring result;
        for (char c : s) {
            result += (wchar_t)c;
        }
        return result;
    }

    void drawExpressionBox() {
        setfillcolor(WHITE);
        setlinecolor(BLACK);
        fillrectangle(EXPR_BOX_X, EXPR_BOX_Y, EXPR_BOX_X + EXPR_BOX_W, EXPR_BOX_Y + EXPR_BOX_H);
        rectangle(EXPR_BOX_X, EXPR_BOX_Y, EXPR_BOX_X + EXPR_BOX_W, EXPR_BOX_Y + EXPR_BOX_H);

        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        settextstyle(34, 0, L"华文行楷");

        int curX = EXPR_BOX_X + 15;
        int ty = EXPR_BOX_Y + (EXPR_BOX_H - textheight(L"0")) / 2;

        bool blinkOn = (int)(clock() / (CLOCKS_PER_SEC / 2)) % 2 == 0;

        for (int i = 0; i <= (int)tokens.size(); i++) {
            if (i == cursorPos && blinkOn) {
                setlinecolor(RGB(30, 100, 200));
                line(curX, EXPR_BOX_Y + 10, curX, EXPR_BOX_Y + EXPR_BOX_H - 10);
            }
            if (i < (int)tokens.size()) {
                wstring t = stringToWstring(tokens[i].text);
                outtextxy(curX, ty, t.c_str());
                curX += textwidth(t.c_str());
            }
        }
    }

    void drawStatusMessage() {
        if (statusMessage.empty()) return;

        setbkmode(TRANSPARENT);
        settextcolor(RGB(200, 30, 30));
        settextstyle(22, 0, L"微软雅黑");

        int tx = (800 - textwidth(statusMessage.c_str())) / 2;
        outtextxy(tx, STATUS_Y, statusMessage.c_str());
    }
};