//自行引入头文件
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <graphics.h>
#include <conio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <mmsystem.h>            
#pragma comment(lib, "winmm.lib")

const int WIN_W = 1600;// 图形窗口的宽度
const int WIN_H = 1000;// 图形窗口的长度

//==========定义==========
#define PLANT_PEASHOOTER 1//豌豆射手
#define PLANT_SUNFLOWER  2//向向葵
#define PLANT_WALLNUT    3//坚果
#define MAX_ENEMY 200
#define ROWS 10
#define COLS 16
#define CELL_SIZE 100
#define WIDTH  (COLS * CELL_SIZE)
#define HEIGHT ( ROWS * CELL_SIZE)
#define SLIDER_LEFT   678
#define SLIDER_RIGHT  1018
#define SLIDER_Y      430

//==========变量==========
int gametime = 0;
enum DIRECTION { left, right, up, down };
int wandoucount = 3;
int wandoumaxcount = 100;
int result;

const wchar_t* MENU_BGM_PATH = L"sucai\\shoye.wav";
const wchar_t* GAME_BGM_PATH = L"sucai\\1.wav";

// ========== 核心坐标换算函数 ==========
int toPixelX(int col) { return col * CELL_SIZE + CELL_SIZE / 2; }
int toPixelY(int row) { return row * CELL_SIZE + CELL_SIZE / 2; }
int toGridX(float pixelX) { return (int)(pixelX / CELL_SIZE); }
int toGridY(float pixelY) { return (int)(pixelY / CELL_SIZE); }

//===========结构体合集============
typedef struct zombie {
    int playerX;
    int playerY;
    int playerHp;
    DIRECTION playerDirection;
    int playerLive;
}z;

int enemyCount = 0;

typedef struct enemy {
    int enemyX;
    int enemyY;
    int enemyHp;
    DIRECTION enemyDirection;
    int enemyLive;
    int enemyType;
}pl;

typedef struct Bullet {
    int bulletX;
    int bulletY;
    DIRECTION bulletDirection;
    int bulletLive;
}b;

typedef struct {
    double rx, ry, rw, rh;
} RectRatio;

const RectRatio PAUSE_BTNS[3] = {
    {0.395, 0.395, 0.210, 0.068},
    {0.395, 0.500, 0.210, 0.068},
    {0.395, 0.610, 0.210, 0.068},
};

const RectRatio VICTORY_BTNS[2] = {
    {0.270, 0.870, 0.190, 0.072},
    {0.540, 0.870, 0.190, 0.072},
};

const RectRatio DEFEAT_BTNS[2] = {
    {0.250, 0.850, 0.190, 0.072},
    {0.560, 0.850, 0.190, 0.072},
};

void RatioToRect(const RectRatio* r, int* x, int* y, int* w, int* h)
{
    *x = (int)(r->rx * WIN_W);
    *y = (int)(r->ry * WIN_H);
    *w = (int)(r->rw * WIN_W);
    *h = (int)(r->rh * WIN_H);
}

int mapName[ROWS][COLS];

int mapLevel1[ROWS][COLS] = {
    {50,1,0,0,0,0,0,0,0,0,0,0,0,0,1,50},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {50,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int mapLevel2[ROWS][COLS] = {
    {50,1,1,0,0,0,0,0,0,0,0,0,0,1,1,50},
    {1,1,0,0,0,0,0,0,0,1,1,1,0,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,1,1,0,1,1,1,0,0,0,0,1,1,0,0},
    {0,0,1,0,0,0,0,0,0,1,1,0,0,1,1,0},
    {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
    {0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0},
    {50,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int mapLevel3[ROWS][COLS] = {
    {50,1,1,1,0,0,0,0,0,0,0,0,1,1,1,50},
    {1,1,0,0,0,0,0,0,0,1,1,1,0,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,0,1,0,0,0,1,1,0,1},
    {0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0},
    {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
    {1,0,1,1,0,0,0,0,0,0,0,0,1,0,0,0},
    {1,0,0,1,1,0,0,0,0,1,1,1,1,0,0,0},
    {1,1,0,0,0,0,0,0,0,1,0,0,0,0,1,1},
    {50,1,1,1,0,0,0,0,0,0,0,0,0,0,1,0}
};

void loadMap(int src[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            mapName[i][j] = src[i][j];
        }
    }
}

int currentMap = 1;
int gameResult = 0;

int score = 0;
int killedEnemyCount = 0;
int playerLife = 3;

int bgmON = 1;
int volume = 50;

// ========== 函数声明 ==========
void DrawMap();
void init(z* p, pl* q);
int isWin(z* p);
int playerMove(z* p, DIRECTION direction);
int enemyMove(pl* r, DIRECTION direction, int a);
DIRECTION changedirection(pl* d, int playerX, int playerY);
int playerShoot(z* p, pl* q);
int bulletMove(b* t, DIRECTION direction, pl* u, z* v);
int bulletgenerate(b* s, int x, int y, DIRECTION direction);
void gezi(int x, int y, int num);
void playBGM(const wchar_t* path, int loop);
void stopBGM();
void setBGMVolume(int volume);
void updateBGMForState(int gameState);

//===============图片合集（扩容以存放掩码图）================
IMAGE zombie[6];
IMAGE wandou[6];
IMAGE sunflower[4];
IMAGE null[4];
IMAGE lightgreen;
IMAGE darkgreen;
IMAGE map;
IMAGE bullet[4];

// -------------------- view 函数声明 --------------------
int menuView(void);
int mapSelectView(void);
int mapLoadingView(int mapIndex);
int pauseView(void);
int winView(void);
int loseView(const wchar_t* reason);
int settingView(void);
int teamView(void);
int gameView(void);

// ============================================================
//  选中高亮框
// ============================================================
void DrawSelectFrame(int x, int y, int w, int h)
{
    int pad = 8;
    setlinecolor(RGB(255, 255, 150));
    setlinestyle(PS_SOLID, 6);
    rectangle(x - pad, y - pad, x + w + pad, y + h + pad);
    setlinecolor(RGB(255, 230, 80));
    setlinestyle(PS_SOLID, 4);
    rectangle(x - pad + 3, y - pad + 3, x + w + pad - 3, y + h + pad - 3);
    setlinecolor(RGB(255, 255, 220));
    setlinestyle(PS_SOLID, 2);
    rectangle(x - pad + 6, y - pad + 6, x + w + pad - 6, y + h + pad - 6);
}

void DrawHint(const wchar_t* text)
{
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 255, 200));
    settextstyle(20, 0, L"黑体");
    int tw = textwidth(text);
    outtextxy((WIN_W - tw) / 2, WIN_H - 32, text);
}

// ============================================================
//  菜单视图 (menuView)
// ============================================================
int menuView(void) {
    IMAGE menuImages[4];
    int selected = 0;
    int stateTable[4] = { 1, 7, 8, 9 };
    loadimage(&menuImages[0], L"res\\menu1.png", WIN_W, WIN_H);
    loadimage(&menuImages[1], L"res\\menu2.png", WIN_W, WIN_H);
    loadimage(&menuImages[2], L"res\\menu3.png", WIN_W, WIN_H);
    loadimage(&menuImages[3], L"res\\menu4.png", WIN_W, WIN_H);
    BeginBatchDraw();
    while (1) {
        putimage(0, 0, &menuImages[selected]);
        FlushBatchDraw();
        ExMessage message = getmessage(EX_KEY);
        if (message.message == WM_KEYDOWN)
        {
            switch (message.vkcode)
            {
            case 'W': selected = (selected + 3) % 4; break;
            case 'S': selected = (selected + 1) % 4; break;
            case VK_RETURN:
                EndBatchDraw();
                return stateTable[selected];
            }
        }
    }
}

// ============================================================
//  地图选择视图 (mapSelectView)
// ============================================================
int mapSelectView(void)
{
    IMAGE mapImages[3];
    int selected = 0;
    loadimage(&mapImages[0], L"res\\map1.png", WIN_W, WIN_H);
    loadimage(&mapImages[1], L"res\\map2.png", WIN_W, WIN_H);
    loadimage(&mapImages[2], L"res\\map3.png", WIN_W, WIN_H);
    BeginBatchDraw();
    while (1)
    {
        putimage(0, 0, &mapImages[selected]);
        FlushBatchDraw();
        ExMessage message = getmessage(EX_KEY);
        if (message.message != WM_KEYDOWN) continue;
        switch (message.vkcode)
        {
        case 'A': selected--; if (selected < 0) selected = 0; break;
        case 'D': selected++; if (selected > 2) selected = 2; break;
        case VK_RETURN:
            currentMap = selected + 1;
            EndBatchDraw();
            return 2;
        case VK_ESCAPE:
            EndBatchDraw();
            return 0;
        }
    }
}

// ============================================================
//  地图加载视图 (mapLoadingView)
// ============================================================
int mapLoadingView(int mapIndex)
{
    printf("\n========== 地图加载中 ==========\n");
    printf("  正在加载: 关卡 %d\n", mapIndex);
    printf("  地图名称: 地图%d\n", mapIndex);
    printf("================================\n\n");
    printf("加载进度: ");
    for (int i = 0; i <= 100; i++)
    {
        if (i % 10 == 0)
        {
            printf("[");
            for (int j = 0; j < 10; j++)
                printf("%c", j < i / 10 ? '#' : '-');
            printf("] %d%%\r", i);
        }
    }
    printf("\n\n地图加载完成！\n");
if(mapIndex == 1) {
        loadMap(mapLevel1);
    }
 else if (mapIndex == 2) {
     loadMap(mapLevel2);
     }
 else if (mapIndex == 3) {
         loadMap(mapLevel3);
         }
 else {
             // 默认加载第一关
             loadMap(mapLevel1);
             }
    return 3;
}

// ============================================================
//  暂停视图 (pauseView)
// ============================================================
int pauseView(void)
{
    IMAGE bg;
    loadimage(&bg, L"res\\pause.jpg", WIN_W, WIN_H);
    int sel = 0;
    const int retVals[3] = { 3, 2, 0 };
    ExMessage msg;
    while (1)
    {
        BeginBatchDraw();
        putimage(0, 0, &bg);
        int x, y, w, h;
        RatioToRect(&PAUSE_BTNS[sel], &x, &y, &w, &h);
        DrawSelectFrame(x, y, w, h);
        DrawHint(L"W/S 或 ↑/↓ 切换    空格确认    ESC继续游戏");
        FlushBatchDraw();
        EndBatchDraw();
        msg = getmessage(EX_KEY);
        if (msg.message == WM_KEYDOWN)
        {
            switch (msg.vkcode)
            {
            case 'W': case VK_UP: sel = (sel + 2) % 3; break;
            case 'S': case VK_DOWN: sel = (sel + 1) % 3; break;
            case 'A': case 'D': case VK_LEFT: case VK_RIGHT: sel = (sel + 1) % 3; break;
            case VK_SPACE: case VK_RETURN: return retVals[sel];
            case VK_ESCAPE: return 3;
            }
        }
    }
}

// ============================================================
//  胜利视图 (winView)
// ============================================================
int winView(void)
{
    IMAGE bg;
    loadimage(&bg, L"res\\victory.png", WIN_W, WIN_H);
    int sel = 0;
    ExMessage msg;
    while (1)
    {
        BeginBatchDraw();
        putimage(0, 0, &bg);
        int x, y, w, h;
        RatioToRect(&VICTORY_BTNS[sel], &x, &y, &w, &h);
        DrawSelectFrame(x, y, w, h);
        setbkmode(TRANSPARENT);
        settextcolor(RGB(255, 230, 150));
        settextstyle(26, 0, L"黑体");
        wchar_t statBuf[128];
        swprintf(statBuf, 128, L"得分: %d", score);
        int tw = textwidth(statBuf);
        outtextxy((WIN_W - tw) / 2, 510, statBuf);
        DrawHint(L"A/D 或 ←/→ 切换    空格确认");
        FlushBatchDraw();
        EndBatchDraw();
        msg = getmessage(EX_KEY);
        if (msg.message == WM_KEYDOWN)
        {
            switch (msg.vkcode)
            {
            case 'A': case 'D': case VK_LEFT: case VK_RIGHT:
            case 'W': case 'S': case VK_UP: case VK_DOWN:
                sel = (sel + 1) % 2; break;
            case VK_SPACE: case VK_RETURN:
                if (sel == 0) {
                    if (currentMap == 1) {
                        loadMap(mapLevel2);
                        currentMap = 2;
                    }
                    else if (currentMap == 2) {
                        loadMap(mapLevel3); 
                        currentMap = 3;
                    }
                    // 重置分数等
                    score = 0;
                    killedEnemyCount = 0;
                    playerLife = 3;
                    EndBatchDraw();
                    return 3;   // 重新开始游戏
                }
                else {
                    EndBatchDraw();
                    return 0;   // 返回主菜单
                }
            }
        }
    }
}

// ============================================================
//  失败视图 (loseView)
// ============================================================
int loseView(const wchar_t* reason)
{
    IMAGE bg;
    loadimage(&bg, L"res\\defeat.jpg", WIN_W, WIN_H);
    int sel = 0;
    ExMessage msg;
    while (1)
    {
        BeginBatchDraw();
        putimage(0, 0, &bg);
        int x, y, w, h;
        RatioToRect(&DEFEAT_BTNS[sel], &x, &y, &w, &h);
        DrawSelectFrame(x, y, w, h);
        if (reason && reason[0])
        {
            setbkmode(TRANSPARENT);
            settextcolor(RGB(200, 160, 160));
            settextstyle(24, 0, L"黑体");
            wchar_t buf[128];
            swprintf(buf, 128, L"原因: %s", reason);
            int tw = textwidth(buf);
            outtextxy((WIN_W - tw) / 2, 490, buf);
        }
        DrawHint(L"A/D 或 ←/→ 切换    空格确认");
        FlushBatchDraw();
        EndBatchDraw();
        msg = getmessage(EX_KEY);
        if (msg.message == WM_KEYDOWN)
        {
            switch (msg.vkcode)
            {
            case 'A': case 'D': case VK_LEFT: case VK_RIGHT:
            case 'W': case 'S': case VK_UP: case VK_DOWN:
                sel = (sel + 1) % 2; break;
            case VK_SPACE: case VK_RETURN:
                if (sel == 0) return 2;
                return 0;
            }
        }
    }
}

// ============================================================
//  设置视图 (settingView)
// ============================================================
void drawSettingView(IMAGE settingImages[], int selected)
{
    int imageIndex;
    int knobX;
    wchar_t volumeText[16];
    if (selected == 2)
    {
        if (bgmON == 1) imageIndex = 4; else imageIndex = 5;
    }
    else
    {
        if (bgmON == 1) imageIndex = selected; else imageIndex = selected + 2;
    }
    putimage(0, 0, &settingImages[imageIndex]);
    swprintf_s(volumeText, 16, L"%d", volume);
    setbkmode(TRANSPARENT);
    settextstyle(34, 0, L"微软雅黑");
    settextcolor(RGB(240, 240, 230));
    outtextxy(820, 365, volumeText);
    knobX = SLIDER_LEFT + volume * (SLIDER_RIGHT - SLIDER_LEFT) / 100;
    setlinecolor(RGB(120, 70, 10));
    setfillcolor(RGB(255, 190, 20));
    solidcircle(knobX, SLIDER_Y, 12);
    circle(knobX, SLIDER_Y, 12);
}

int settingView(void)
{
    IMAGE settingImages[6];
    int selected = 0;
    loadimage(&settingImages[0], L"res\\setting1.png", WIN_W, WIN_H);
    loadimage(&settingImages[1], L"res\\setting2.png", WIN_W, WIN_H);
    loadimage(&settingImages[2], L"res\\setting3.png", WIN_W, WIN_H);
    loadimage(&settingImages[3], L"res\\setting4.png", WIN_W, WIN_H);
    loadimage(&settingImages[4], L"res\\setting5.png", WIN_W, WIN_H);
    loadimage(&settingImages[5], L"res\\setting6.png", WIN_W, WIN_H);
    BeginBatchDraw();
    while (1)
    {
        drawSettingView(settingImages, selected);
        FlushBatchDraw();
        ExMessage message = getmessage(EX_KEY);
        if (message.message != WM_KEYDOWN) continue;
        switch (message.vkcode)
        {
        case 'W': selected--; if (selected < 0) selected = 2; break;
        case 'S': selected++; if (selected > 2) selected = 0; break;
        case 'A':
            if (selected == 0) bgmON = 0; else { volume -= 10; if (volume < 0) volume = 0; }
            break;
        case 'D':
            if (selected == 0) bgmON = 1; else { volume += 10; if (volume > 100) volume = 100; }
            break;
        case VK_RETURN:
            if (selected == 2) { EndBatchDraw(); return 0; }
        }
    }
}

// ============================================================
//  团队视图 (teamView) - 修复乱码
// ============================================================
int teamView(void)
{
    IMAGE teamImage;
    loadimage(&teamImage, L"res\\team.png", WIN_W, WIN_H); // 如果图片本身有乱码请自行更换图片

    BeginBatchDraw();
    cleardevice();
    putimage(0, 0, &teamImage);

    setbkmode(TRANSPARENT);           // 文字背景透明
    settextcolor(RGB(255, 255, 200)); // 设置文字颜色
    settextstyle(24, 0, L"黑体");

    wchar_t hint[] = L"按回车键返回菜单";
    int tw = textwidth(hint);
    outtextxy((WIN_W - tw) / 2, WIN_H - 50, hint);

    FlushBatchDraw();

    while (1)
    {
        ExMessage message = getmessage(EX_KEY);
        if (message.message == WM_KEYDOWN)
        {
            // 支持回车(Enter) 和 退出键(Esc) 返回
            if (message.vkcode == VK_RETURN || message.vkcode == VK_ESCAPE)
            {
                EndBatchDraw();
                return 0;
            }
        }
    }
}

// ============================================================
//  ★★★★★ 核心游戏视图 (gameView) ★★★★★
// ============================================================
int gameView(void)
{
    DrawMap();
    gametime = 0;
    z player;
    pl enemy[MAX_ENEMY];
    b bullet[10];
    init(&player, enemy);
    for (int i = 0; i < 10; i++) bullet[i].bulletLive = 0;

    while (1)
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            return 4;
        }

        if (gametime % 6 == 0) {
            for (int i = 0; i < wandoucount; i++) {
                if (enemy[i].enemyLive == 1) {
                    enemyMove(&enemy[i], enemy[i].enemyDirection, i);
                }
            }
        }
        if (gametime % 1 == 0 && gametime % 6 != 0) {
            for (int i = 0; i < wandoucount; i++) {
                if (enemy[i].enemyLive == 1) {
                    enemy[i].enemyDirection = changedirection(&enemy[i], player.playerX, player.playerY);
                }
            }
        }
        if (gametime % 10 == 0) {
            for (int i = 0; i < wandoucount; i++) {
                if (enemy[i].enemyLive == 1 && bullet[i].bulletLive == 0) {
                    bulletgenerate(&bullet[i], enemy[i].enemyX,
                        enemy[i].enemyY, enemy[i].enemyDirection);
                }
            }
        }
        if (gametime % 3 == 0) {
            for (int i = 0; i < 10; i++) {
                if (bullet[i].bulletLive == 1) {
                    result = bulletMove(&bullet[i], bullet[i].bulletDirection, enemy, &player);
                }
            }
        }
        if (player.playerLive == 1) {
            ExMessage msg;
            while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
                switch (msg.message)
                {
                case WM_KEYDOWN:
                    switch (msg.vkcode)
                    {
                    case 'A': case 'a': playerMove(&player, left); break;
                    case 'W': case 'w': playerMove(&player, up); break;
                    case 'S': case 's': playerMove(&player, down); break;
                    case 'D': case 'd': playerMove(&player, right); break;
                    case 'J': case 'j': playerShoot(&player, enemy); break;
                    case 'P': case 'p': system("pause"); break;
                    default: break;
                    }
                    break;
                }
            }
        }
        result = isWin(&player);
        if (result == 1) {
            score = 100;
            killedEnemyCount = 5;
            playerLife = 3;
            return 5;
        }
        else if (result == 2) {
            return 6;
        }

        // 血条
        int barW = 200, barH = 24;
        int barX = (WIN_W - barW) / 2;   
        int barY = 20;                    

        setfillcolor(RGB(0, 0, 0));
        solidrectangle(barX - 10, barY - 10, barX + barW + 10, barY + barH + 30);

        setfillcolor(RGB(60, 60, 60));
        solidrectangle(barX, barY, barX + barW, barY + barH);

        int hpPercent = player.playerHp;
        if (hpPercent < 0) hpPercent = 0;
        if (hpPercent > 100) hpPercent = 100;   // 保险

        setfillcolor(RGB(255, 0, 0));
        solidrectangle(barX, barY, barX + (int)(barW * hpPercent / 100.0), barY + barH);

        setlinecolor(RGB(255, 255, 255));
        rectangle(barX, barY, barX + barW, barY + barH);

        setbkmode(TRANSPARENT);
        settextcolor(RGB(255, 255, 255));
        settextstyle(18, 0, L"黑体");
        wchar_t hpText[32];
        swprintf(hpText, 32, L"僵尸HP: %d/100", player.playerHp);
        int tw = textwidth(hpText);
        int th = textheight(hpText);
        outtextxy(barX + (barW - tw) / 2, barY + (barH - th) / 2, hpText);

        Sleep(100);
        gametime++;
    }
}

// ============================================================
//  以下为移植的函数实现
// ============================================================

//========== 初始化角色(僵尸植物) ==========
void init(z* p, pl* q) {
    //=====加载图片（原图与掩码图）=====
    loadimage(&zombie[left], L"sucai\\zombieleft_bg.png", CELL_SIZE, CELL_SIZE);
    loadimage(&zombie[4], L"sucai\\zombieleft_mask.png", CELL_SIZE, CELL_SIZE);

    loadimage(&zombie[right], L"sucai\\zombieright_bg.png", CELL_SIZE, CELL_SIZE);
    loadimage(&zombie[5], L"sucai\\zombieright_mask.png", CELL_SIZE, CELL_SIZE);

    loadimage(&wandou[right], L"sucai\\wandouright_bg.png", CELL_SIZE, CELL_SIZE);
    loadimage(&wandou[3], L"sucai\\wandouright_mask.png", CELL_SIZE, CELL_SIZE);

    loadimage(&wandou[left], L"sucai\\wandouleft_bg.png", CELL_SIZE, CELL_SIZE);
    loadimage(&wandou[4], L"sucai\\wandouleft_mask.png", CELL_SIZE, CELL_SIZE);

    loadimage(&null[0], L"sucai\\null1_bg.png", CELL_SIZE, CELL_SIZE);
    loadimage(&null[1], L"sucai\\null1_mask.png", CELL_SIZE, CELL_SIZE);

    loadimage(&sunflower[1], L"sucai\\sunflower_bg.png", CELL_SIZE, CELL_SIZE);
    loadimage(&sunflower[2], L"sucai\\sunflower_mask.png", CELL_SIZE, CELL_SIZE);

    loadimage(&lightgreen, L"sucai\\lightgreen.png", CELL_SIZE, CELL_SIZE);

    loadimage(&bullet[1], L"sucai\\bullet_bg.png", CELL_SIZE, CELL_SIZE);
    loadimage(&bullet[2], L"sucai\\bullet_mask.png", CELL_SIZE, CELL_SIZE);

    int index = wandoumaxcount + 1;

    //初始化玩家(僵尸)
    p->playerX = 15;
    p->playerY = 9;
    p->playerHp = 100;
    p->playerLive = 1;
    p->playerDirection = left;
    gezi(p->playerX, p->playerY, 200);

    // 使用掩码贴图
    putimage(p->playerX * CELL_SIZE, p->playerY * CELL_SIZE, &zombie[4], SRCAND);
    putimage(p->playerX * CELL_SIZE, p->playerY * CELL_SIZE, &zombie[left], SRCPAINT);

    //初始化敌人(豌豆射手)
    int i;
    for (i = 0; i < wandoucount; i++) {
        if (i % 3 == 0) {
            q[i].enemyX = 4; q[i].enemyY = 3; q[i].enemyHp = 100;
            putimage(q[i].enemyX * CELL_SIZE, q[i].enemyY * CELL_SIZE, &wandou[3], SRCAND);
            putimage(q[i].enemyX * CELL_SIZE, q[i].enemyY * CELL_SIZE, &wandou[right], SRCPAINT);
            mapName[q[i].enemyY][q[i].enemyX] = 100 + i;
            q[i].enemyDirection = right; q[i].enemyLive = 1; q[i].enemyType = PLANT_PEASHOOTER;
            gezi(q[i].enemyX, q[i].enemyY, 100 + i);
        }
        else if (i % 3 == 1) {
            q[i].enemyX = 9; q[i].enemyY = 7; q[i].enemyHp = 100;
            putimage(q[i].enemyX * CELL_SIZE, q[i].enemyY * CELL_SIZE, &wandou[3], SRCAND);
            putimage(q[i].enemyX * CELL_SIZE, q[i].enemyY * CELL_SIZE, &wandou[right], SRCPAINT);
            mapName[q[i].enemyY][q[i].enemyX] = 100 + i;
            q[i].enemyDirection = right; q[i].enemyLive = 1; q[i].enemyType = PLANT_PEASHOOTER;
            gezi(q[i].enemyX, q[i].enemyY, 100 + i);
        }
        else if (i % 3 == 2) {
            q[i].enemyX = 8; q[i].enemyY = 8; q[i].enemyHp = 100;
            putimage(q[i].enemyX * CELL_SIZE, q[i].enemyY * CELL_SIZE, &wandou[3], SRCAND);
            putimage(q[i].enemyX * CELL_SIZE, q[i].enemyY * CELL_SIZE, &wandou[right], SRCPAINT);
            mapName[q[i].enemyY][q[i].enemyX] = 100 + i;
            q[i].enemyDirection = right; q[i].enemyLive = 1; q[i].enemyType = PLANT_PEASHOOTER;
            gezi(q[i].enemyX, q[i].enemyY, 100 + i);
        }
    }

    //=====遍历数组初始化向日葵坚果墙=====
    for (int y = 0; y < ROWS; y++)
    {
        for (int x = 0; x < COLS; x++)
        {
            if (mapName[y][x] == 50) {
                q[index].enemyX = x; q[index].enemyY = y; q[index].enemyHp = 100;
                q[index].enemyLive = 1; q[index].enemyType = PLANT_SUNFLOWER;
                putimage(x * CELL_SIZE, y * CELL_SIZE, &sunflower[2], SRCAND);
                putimage(x * CELL_SIZE, y * CELL_SIZE, &sunflower[1], SRCPAINT);
                index++;
            }
            else if (mapName[y][x] == 1) {
                q[index].enemyX = x; q[index].enemyY = y; q[index].enemyHp = 300;
                q[index].enemyLive = 1; q[index].enemyType = PLANT_WALLNUT;
                putimage(x * CELL_SIZE, y * CELL_SIZE, &null[1], SRCAND);
                putimage(x * CELL_SIZE, y * CELL_SIZE, &null[0], SRCPAINT);
                index++;
            }
        }
    }
    enemyCount = index - wandoumaxcount + wandoucount;
}

// ========== 角色移动 ==========
int playerMove(z* p, DIRECTION direction) {
    int nx = p->playerX, ny = p->playerY;
    if (direction == left)  nx--;
    if (direction == right) nx++;
    if (direction == up)    ny--;
    if (direction == down)  ny++;
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return 0;
    if (mapName[ny][nx] != 0) return 0;

    putimage(p->playerX * CELL_SIZE, p->playerY * CELL_SIZE, &lightgreen);
    gezi(p->playerX, p->playerY, 0);
    p->playerX = nx; p->playerY = ny;
    gezi(p->playerX, p->playerY, 200);

    if (direction == left || direction == right) p->playerDirection = direction;
    if (p->playerDirection == left)
    {
        putimage(p->playerX * CELL_SIZE, p->playerY * CELL_SIZE, &zombie[4], SRCAND);
        putimage(p->playerX * CELL_SIZE, p->playerY * CELL_SIZE, &zombie[left], SRCPAINT);
    }
    else if (p->playerDirection == right)
    {
        putimage(p->playerX * CELL_SIZE, p->playerY * CELL_SIZE, &zombie[5], SRCAND);
        putimage(p->playerX * CELL_SIZE, p->playerY * CELL_SIZE, &zombie[right], SRCPAINT);
    }
    return 1;
}

//===========敌人(植物)移动=============
int enemyMove(pl* r, DIRECTION direction, int a) {
    int nx = r->enemyX, ny = r->enemyY;
    switch (direction) {
    case right: nx++; break;
    case left:  nx--; break;
    case up:    ny--; break;
    case down:  ny++; break;
    }
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return 0;
    if (mapName[ny][nx] != 0) return 0;

    putimage(r->enemyX * CELL_SIZE, r->enemyY * CELL_SIZE, &lightgreen);
    gezi(r->enemyX, r->enemyY, 0);
    r->enemyX = nx; r->enemyY = ny;
    gezi(r->enemyX, r->enemyY, 100 + a);

    if (direction == left || direction == right) r->enemyDirection = direction;
    if (r->enemyDirection == left)
    {
        putimage(r->enemyX * CELL_SIZE, r->enemyY * CELL_SIZE, &wandou[4], SRCAND);
        putimage(r->enemyX * CELL_SIZE, r->enemyY * CELL_SIZE, &wandou[left], SRCPAINT);
    }
    else
    {
        putimage(r->enemyX * CELL_SIZE, r->enemyY * CELL_SIZE, &wandou[3], SRCAND);
        putimage(r->enemyX * CELL_SIZE, r->enemyY * CELL_SIZE, &wandou[right], SRCPAINT);
    }
    return 1;
}

//============植物变向===========
DIRECTION changedirection(pl* d, int playerX, int playerY) {
    int num = rand() % 100;
    if (num > 0 && num < 50) {
        if (playerX > d->enemyX) {
            putimage(d->enemyX * CELL_SIZE, d->enemyY * CELL_SIZE, &lightgreen);
            putimage(d->enemyX * CELL_SIZE, d->enemyY * CELL_SIZE, &wandou[3], SRCAND);
            putimage(d->enemyX * CELL_SIZE, d->enemyY * CELL_SIZE, &wandou[right], SRCPAINT);
            return right;
        }
        else if (playerX < d->enemyX)
        {
            putimage(d->enemyX * CELL_SIZE, d->enemyY * CELL_SIZE, &lightgreen);
            putimage(d->enemyX * CELL_SIZE, d->enemyY * CELL_SIZE, &wandou[4], SRCAND);
            putimage(d->enemyX * CELL_SIZE, d->enemyY * CELL_SIZE, &wandou[left], SRCPAINT);
            return left;
        }
        else return d->enemyDirection;
    }
    else {
        if (playerY > d->enemyY) return down;
        else if (playerY < d->enemyY) return up;
        else return d->enemyDirection;
    }
}

//=======子弹生成=======
int bulletgenerate(b* s, int x, int y, DIRECTION direction) {
    int nx = x, ny = y;
    switch (direction) {
    case right: nx++; break;
    case left:  nx--; break;
    case up:    ny--; break;
    case down:  ny++; break;
    }
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return 0;
    if (mapName[ny][nx] != 0) return 0;
    putimage(nx * CELL_SIZE, ny * CELL_SIZE, &bullet[2], SRCAND);
    putimage(nx * CELL_SIZE, ny * CELL_SIZE, &bullet[1], SRCPAINT);
    s->bulletX = nx; s->bulletY = ny; s->bulletLive = 1; s->bulletDirection = direction;
    return 1;
}

//=========子弹移动==========
int bulletMove(b* t, DIRECTION direction, pl* u, z* v) {
    int nx = t->bulletX, ny = t->bulletY;
    switch (direction) {
    case right: nx++; break;
    case left:  nx--; break;
    case up:    ny--; break;
    case down:  ny++; break;
    }
    putimage(t->bulletX * CELL_SIZE, t->bulletY * CELL_SIZE, &lightgreen);
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) { t->bulletLive = 0; return 0; }
    if (mapName[ny][nx] != 0) {
        if (mapName[ny][nx] == 200) {
            v->playerHp -= 25;
            if (v->playerHp <= 0) { v->playerLive = 0; putimage(nx * CELL_SIZE, ny * CELL_SIZE, &lightgreen); return 0; }
            return 0;
        }
        else { t->bulletLive = 0; return 0; }
    }
    t->bulletX = nx; t->bulletY = ny;
    putimage(t->bulletX * CELL_SIZE, t->bulletY * CELL_SIZE, &bullet[2], SRCAND);
    putimage(t->bulletX * CELL_SIZE, t->bulletY * CELL_SIZE, &bullet[1], SRCPAINT);
    return 0;
}

//===========啃咬===============
int playerShoot(z* p, pl* q) {
    int hx = p->playerX, hy = p->playerY;
    if (p->playerDirection == left) hx--;
    else if (p->playerDirection == right) hx++;
    if (hx < 0 || hx >= COLS || hy < 0 || hy >= ROWS) return 0;
    for (int i = 0; i < 200; i++) {
        if (!q[i].enemyLive) continue;
        if (q[i].enemyLive && q[i].enemyX == hx && q[i].enemyY == hy) {
            q[i].enemyHp -= 50;
            if (q[i].enemyHp <= 0) {
                q[i].enemyLive = 0;
                mapName[hy][hx] = 0;
                putimage(hx * CELL_SIZE, hy * CELL_SIZE, &lightgreen);
            }
            return 1;
        }
    }
    return 0;
}

//===========胜利判断=========
int isWin(z* p) {
    if (mapName[0][0] == 0 && mapName[0][15] == 0 && mapName[9][0] == 0)
        return 1;
    if (p->playerHp <= 0)
        return 2;
    else
        return 0;
}

// ========== 绘制地图 ==========
void DrawMap() {
    loadimage(&map, L"sucai\\map.png", WIDTH, HEIGHT);
    putimage(0, 0, &map);
}

//======玩家和角色移动对于数组数字的改变======
void gezi(int x, int y, int num) {
    mapName[y][x] = num;
}

//  音乐控制函数实现（完整保留）
// ============================================================
void playBGM(const wchar_t* path, int loop)
{
    if (!bgmON)
        return;

    mciSendStringW(L"stop bgm", NULL, 0, NULL);
    mciSendStringW(L"close bgm", NULL, 0, NULL);

    wchar_t openCmd[512];
    swprintf_s(openCmd, 512, L"open \"%ls\" type mpegvideo alias bgm", path);

    MCIERROR error = mciSendStringW(openCmd, NULL, 0, NULL);

    if (error != 0)
    {
        wchar_t errorText[256];
        mciGetErrorStringW(error, errorText, 256);
        MessageBoxW(NULL, errorText, L"音乐打开失败", MB_OK);
        return;
    }

    setBGMVolume(volume);

    if (loop)
        error = mciSendStringW(L"play bgm from 0 repeat", NULL, 0, NULL);
    else
        error = mciSendStringW(L"play bgm from 0", NULL, 0, NULL);

    if (error != 0)
    {
        wchar_t errorText[256];
        mciGetErrorStringW(error, errorText, 256);
        MessageBoxW(NULL, errorText, L"音乐播放失败", MB_OK);
    }
}

void stopBGM()
{
    mciSendString(L"stop bgm", NULL, 0, NULL);
    mciSendString(L"close bgm", NULL, 0, NULL);
}

void setBGMVolume(int vol)
{
    int mciVol = vol * 10;
    wchar_t cmd[64];
    swprintf(cmd, 64, L"setaudio bgm volume to %d", mciVol);
    mciSendString(cmd, NULL, 0, NULL);
}

void updateBGMForState(int gameState)
{
    if (gameState == 3) {
        playBGM(GAME_BGM_PATH, 1);
    }
    else {
        playBGM(MENU_BGM_PATH, 1);
    }
}

// ============================================================
//  主函数
// ============================================================
int main(void)
{
    int gameState;
    srand((unsigned int)time(NULL));
    initgraph(1600, 1000);
    gameState = 0;
    int lastState = -1;
    while (gameState != 9) {
        if (gameState != lastState) {
            updateBGMForState(gameState);
            lastState = gameState;
        }
        switch (gameState) {
        case 0: gameState = menuView(); break;
        case 1: gameState = mapSelectView(); break;
        case 2: gameState = mapLoadingView(currentMap); break;
        case 3: gameState = gameView(); break;
        case 4: gameState = pauseView(); break;
        case 5: gameState = winView(); break;
        case 6: gameState = loseView(L"玩家被僵尸吃掉了脑子"); break;
        case 7: gameState = settingView(); break;
        case 8: gameState = teamView(); break;
        default: gameState = 0; break;
        }
    }
    stopBGM();
    closegraph();
    return 0;
}