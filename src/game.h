#ifndef CGAME_H
#define CGAME_H
#include <cstdint>
#include <vector>
#include "shape.h"
#include "grid.h"

class CFrameSet;
class CFrame;
class CFont;
class CGrid;
class CShape;

class CGame
{
public:
    explicit CGame();
    virtual ~CGame();

    void init();
    void run();

protected:
    typedef struct
    {
        int x;
        int y;
        int width;
        int height;
    } Rect;

    void newShape();
    void drawShape(bool erase = false);
    void eraseShape();
    bool canMoveShape(int aim);
    void collapseCol(int16_t x);
    void removePeers(peers_t &peers);
    void collapseCols(std::set<int16_t> &chCols);
    void blocksFromShape(CShape &shape, std::vector<pos_t> &blocks);
    void blocksFromCols(std::set<int16_t> &chCols, std::vector<pos_t> &blocks);
    uint16_t managePeers(CShape &shape);
    void initGame();
    void manageGame();
    virtual void paint();
    void drawScreen(CFrame &bitmap);
    void preloadAssets();
    inline void drawRect(CFrame &frame, const Rect &rect, const uint32_t color = GREEN, bool fill = true);
    inline void drawTile(CFrame &frame, const int x, const int y, CFrame &tile, bool alpha);
    inline void drawString(CFrame &bitmap, const int x, const int y, const char *s, const uint32_t color = WHITE, const uint32_t bkcolor = BLACK);
    inline void drawStatus(CFrame &bitmap);
    void startCountdown(int f = 1);
    void test();
    void restartGame();

    enum : uint32_t
    {
        TICK_RATE = 24,
        NO_ANIMZ = 255,
        KEY_PRESSED = 1,
        KEY_RELEASED = 0,
        INTRO_DELAY = TICK_RATE,
        ALPHA = 0xff000000,
        WHITE = 0x00ffffff | ALPHA,
        YELLOW = 0x0000ffff | ALPHA,
        PURPLE = 0x00ff00ff | ALPHA,
        BLACK = 0x00000000 | ALPHA,
        GREEN = 0x0000ff00 | ALPHA,
        LIME = 0x0000ffbf | ALPHA,
        BLUE = 0x00ff0000 | ALPHA,
        DARKBLUE = 0x00440000 | ALPHA,
        MEDIUM_BLUE = 0x00880000 | ALPHA,
        DARKGRAY = 0x00444444 | ALPHA,
        LIGHTGRAY = 0x00A9A9A9 | ALPHA,
        CYAN = 0x00aaaa00 | ALPHA,
        WIDTH = 240,
        HEIGHT = 320,
        TILE_SIZE = 16,
        COUNTDOWN_INTRO = 1,
        COUNTDOWN_RESTART = 2,
        TILE_BLANK = 0,
        FONT_SIZE = 8,
        AIM_UP = 0,
        AIM_DOWN = 1,
        AIM_LEFT = 2,
        AIM_RIGHT = 3,
    };

    enum : uint16_t
    {
        TILE_WIDTH = 16,
        GRIDSIZE = TILE_WIDTH,
        COLS = WIDTH / GRIDSIZE,
        ROWS = HEIGHT / GRIDSIZE,
        BLOCKS_PER_LEVEL = 50,
        LEVEL_BONUS = 25,
        SPEED_OFFSET = 2,
    };

    enum : int32_t
    {
        INVALID = -1
    };

    uint8_t m_joyState[4];
    uint64_t m_ticks = 0;
    CFrameSet *m_blocks = nullptr;
    CFont *m_font;
    CGrid *m_grid;
    int m_countdown = 0;
    uint16_t m_gameSpeed;
    uint32_t m_score;
    uint16_t m_level;
    uint16_t m_blockCount;
    uint16_t m_totalBlocks;
    int m_blockRange;
    CShape m_shape;
};

#endif // CGAMEMIXIN_H
