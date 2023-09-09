#include "shared/FrameSet.h"
#include "shared/Frame.h"
#include "game.h"
#include "shared/FileWrap.h"
#include "font.h"
#include "grid.h"
#include "shape.h"
#include <cstring>

CGame::CGame()
{
    m_grid = new CGrid(COLS, ROWS);
    m_font = new CFont(CFont::shift8bytes);
    preloadAssets();
    memset(m_joyState, 0, sizeof(m_joyState));
}

CGame::~CGame()
{
    if (m_blocks)
    {
        delete m_blocks;
    }

    if (m_font)
    {
        delete m_font;
    }
}

void CGame::preloadAssets()
{
    CFileWrap file;

    typedef struct
    {
        const char *filename;
        CFrameSet **frameset;
    } asset_t;

    asset_t assets[] = {
        {"data/blocks.obl", &m_blocks},
    };

    for (int i = 0; i < 1; ++i)
    {
        asset_t &asset = assets[i];
        *(asset.frameset) = new CFrameSet();
        if (file.open(asset.filename, "rb"))
        {
            printf("reading %s\n", asset.filename);
            if ((*(asset.frameset))->extract(file))
            {
                printf("extracted: %d\n", (*(asset.frameset))->getSize());
            }
            file.close();
        }
    }

    const char *fontName = "data/bitfont.bin";
    printf("loading font: %s\n", fontName);
    if (!m_font->read(fontName))
    {
        printf("failed to open %s\n", fontName);
    }

    init();
    //  test();
}

void CGame::drawFont(CFrame &frame, int x, int y, const char *text, const uint32_t color)
{
    uint32_t *rgba = frame.getRGB();
    const int rowPixels = frame.len();
    const int fontSize = 8;
    const int fontOffset = fontSize * fontSize;
    const int textSize = strlen(text);
    for (int i = 0; i < textSize; ++i)
    {
        const uint8_t c = static_cast<uint8_t>(text[i]) - ' ';
        uint8_t *font = m_fontData + c * fontOffset;
        for (int yy = 0; yy < fontSize; ++yy)
        {
            for (int xx = 0; xx < fontSize; ++xx)
            {
                rgba[(yy + y) * rowPixels + xx + x] = *font ? color : BLACK;
                ++font;
            }
        }
        x += fontSize;
    }
}

void CGame::drawRect(CFrame &frame, const Rect &rect, const uint32_t color, bool fill)
{
    uint32_t *rgba = frame.getRGB();
    const int rowPixels = frame.len();
    if (fill)
    {
        for (int y = 0; y < rect.height; y++)
        {
            for (int x = 0; x < rect.width; x++)
            {
                rgba[(rect.y + y) * rowPixels + rect.x + x] = color;
            }
        }
    }
    else
    {
        for (int y = 0; y < rect.height; y++)
        {
            for (int x = 0; x < rect.width; x++)
            {
                if (y == 0 || y == rect.height - 1 || x == 0 || x == rect.width - 1)
                {
                    rgba[(rect.y + y) * rowPixels + rect.x + x] = color;
                }
            }
        }
    }
}

void CGame::drawTile(CFrame &bitmap, const int x, const int y, CFrame &tile, bool alpha)
{
    const uint32_t *tileData = tile.getRGB();
    uint32_t *dest = bitmap.getRGB() + x + y * WIDTH;
    if (alpha)
    {
        for (uint32_t row = 0; row < TILE_SIZE; ++row)
        {
            for (uint32_t col = 0; col < TILE_SIZE; ++col)
            {
                const uint32_t &rgba = tileData[col];
                if (rgba & ALPHA)
                {
                    dest[col] = rgba;
                }
            }
            dest += WIDTH;
            tileData += TILE_SIZE;
        }
    }
    else
    {
        for (uint32_t row = 0; row < TILE_SIZE; ++row)
        {
            int i = 0;
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);

            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);

            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);

            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest[i++] = *(tileData++);
            dest += WIDTH;
        }
    }
}

void CGame::mainLoop()
{

    if (m_countdown > 0)
    {
        --m_countdown;
    }

    ++m_ticks;
    manageGame();
}

void CGame::startCountdown(int f)
{
    m_countdown = f * INTRO_DELAY;
}

void CGame::drawScreen(CFrame &bitmap)
{
    CFrameSet &blocks = *m_blocks;
    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < COLS; ++x)
        {
            drawTile(bitmap, x * TILE_SIZE, y * TILE_SIZE, *blocks[m_grid->at(x, y)], false);
        }
    }

    drawStatus(bitmap);
}

void CGame::test()
{
    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < COLS; ++x)
        {
            m_grid->at(x, y) = ::random() % m_blocks->getSize();
        }
    }
}

inline void CGame::drawString(CFrame &bitmap, const int x, const int y, const char *s, const uint32_t color, const uint32_t bkcolor)
{
    const int fontSize = 8;
    for (int i = 0; s[i]; ++i)
    {
        int j = s[i] - 32;
        const uint8_t *p = m_font->get(j);
        uint32_t *rgba = bitmap.getRGB() + x + i * fontSize + y * bitmap.len();
        for (int row = 0; row < fontSize; ++row)
        {
            uint8_t c = p[row];
            for (int col = 0; col < fontSize; ++col)
            {
                rgba[col] = c & 1 ? color : bkcolor;
                c = c >> 1;
            }
            rgba += bitmap.len();
        }
    }
}

void CGame::drawStatus(CFrame &bitmap)
{
    char t[16];
    const int fontSize = 8;
    // Score
    drawString(bitmap, 0, 0, "SCORE", CYAN, MEDIUM_BLUE);
    sprintf(t, "%.6ld", score);
    drawString(bitmap, 0, fontSize, t, CYAN, MEDIUM_BLUE);

    // Level
    drawString(bitmap, 7 * fontSize, 0, "LEVEL", WHITE, MEDIUM_BLUE);
    sprintf(t, "  %.2d ", level);
    drawString(bitmap, 7 * fontSize, fontSize, t, WHITE, MEDIUM_BLUE);

    // Blocks Left
    drawString(bitmap, 14 * fontSize, 0, "LEFT", PURPLE, MEDIUM_BLUE);
    sprintf(t, " %.2d ", blocksPerLevel - blockCount);
    drawString(bitmap, 14 * fontSize, fontSize, t, PURPLE, MEDIUM_BLUE);
}

void CGame::init()
{
    printf("CGame::init()\n");
    gameSpeed = 50;
    score = 0;
    level = 1;
    blockCount = 0;
    totalBlocks = 0;
    blockRange = CShape::DEFAULT_RANGE;
    m_grid->clear();
    initGame();
}

void CGame::newShape()
{
    m_shape.newShape(random() % COLS, -2, blockRange);
    drawShape();
}

void CGame::restartGame()
{
    newShape();
}

void CGame::drawShape(bool erase)
{
    CShape &shape = m_shape;
    uint8_t x = shape.x();
    for (int8_t i = 0; i < CShape::height(); ++i)
    {
        int8_t y = shape.y() + i;
        if (y < 0)
        {
            continue;
        }
        uint8_t tile = erase ? TILE_BLANK : shape.tile(i);
        m_grid->at(x, y) = tile;
    }
}

void CGame::eraseShape()
{
    drawShape(true);
}

bool CGame::canMoveShape(int aim)
{
    CShape &shape = m_shape;
    CGrid &grid = *m_grid;
    int x = shape.x();
    int y = shape.y();
    int i;

    switch (aim)
    {
    case CShape::DOWN:
        y += shape.height();
        return y < rows && grid.at(x, y) == TILE_BLANK;
    case CShape::LEFT:
        --x;
        for (i = 0; i < shape.height(); ++i)
        {
            if (x < 0)
            {
                return false;
            }
            if (i + y < 0)
            {
                continue;
            }
            if (grid.at(x, i + y) != TILE_BLANK)
            {
                return false;
            };
        }
        return true;
    case CShape::RIGHT:
        ++x;
        for (i = 0; i < shape.height(); ++i)
        {
            if (x >= cols)
            {
                return false;
            }
            if (i + y < 0)
            {
                continue;
            }
            if (grid.at(x, i + y) != TILE_BLANK)
            {
                return false;
            };
        }
        return true;
    }
    return false;
}

void CGame::collapseCol(int16_t x)
{
    CShape &shape = m_shape;
    CGrid &grid = *m_grid;
    int16_t dy = INVALID;
    for (int16_t y = rows - 1; y >= 0; --y)
    {
        if ((dy == INVALID) && grid.at(x, y) == TILE_BLANK)
        {
            dy = y;
            continue;
        }

        if (grid.at(x, y) != TILE_BLANK && dy != INVALID)
        {
            grid.at(x, dy) = grid.at(x, y);
            //   drawTile(x, dy, grid.at(x, y));
            --dy;
            grid.at(x, y) = TILE_BLANK;
            //  drawTile(x, y, TILE_BLANK);
        }
    }
}

void CGame::removePeers(peers_t &peers)
{
    CShape &shape = m_shape;
    CGrid &grid = *m_grid;
    for (std::set<uint32_t>::iterator it = peers.begin(); it != peers.end(); ++it)
    {
        uint32_t key = *it;
        pos_t p = CGrid::toPos(key);
        grid.at(p.x, p.y) = TILE_BLANK;
    }
}

void CGame::collapseCols(std::set<int16_t> &chCols)
{
    CShape &shape = m_shape;
    CGrid &grid = *m_grid;
    for (std::set<int16_t>::iterator it = chCols.begin(); it != chCols.end(); ++it)
    {
        int16_t x = *it;
        collapseCol(x);
    }
}

void CGame::blocksFromShape(CShape &shape, std::vector<pos_t> &blocks)
{
    CGrid &grid = *m_grid;
    blocks.clear();
    uint8_t x = shape.x();
    for (int i = 0; i < shape.height(); ++i)
    {
        uint8_t y = i + shape.y();
        if (grid.isValidPos(x, y) && grid.at(x, y))
        {
            pos_t block = {x, y};
            blocks.push_back(block);
        }
    }
}

void CGame::blocksFromCols(std::set<int16_t> &chCols, std::vector<pos_t> &blocks)
{
    CShape &shape = m_shape;
    CGrid &grid = *m_grid;
    blocks.clear();
    for (std::set<int16_t>::iterator it = chCols.begin(); it != chCols.end(); ++it)
    {
        int16_t x = *it;
        for (int16_t y = 0; y < rows; ++y)
        {
            if (grid.at(x, y) != TILE_BLANK)
            {
                pos_t block = {x, y};
                blocks.push_back(block);
            }
        }
    }
}

uint16_t CGame::managePeers(CShape &shape)
{
    CGrid &grid = *m_grid;
    uint16_t removedBlocks = 0;
    std::vector<pos_t> blocks;
    blocksFromShape(shape, blocks);
    std::set<int16_t> chCols;
    peers_t allPeers;
    while (blocks.size() != 0)
    {
        for (auto p = blocks.begin(); p != blocks.end(); ++p)
        {
            peers_t peers;
            pos_t pos = *p;
            grid.findPeers(pos.x, pos.y, peers);
            if (peers.size() >= 3)
            {
                for (std::set<uint32_t>::iterator it = peers.begin(); it != peers.end(); ++it)
                {
                    uint32_t key = *it;
                    pos_t p = CGrid::toPos(key);
                    if (chCols.count(p.x) == 0)
                    {
                        chCols.insert(p.x);
                    }
                    if (allPeers.count(key) == 0)
                    {
                        allPeers.insert(key);
                    }
                }
            }
        }
        removePeers(allPeers);
        removedBlocks += allPeers.size();
        //  vTaskDelay(50 / portTICK_PERIOD_MS);
        collapseCols(chCols);
        //  vTaskDelay(50 / portTICK_PERIOD_MS);
        blocksFromCols(chCols, blocks);
        allPeers.clear();
        chCols.clear();
    }
    return removedBlocks;
}

void CGame::initGame()
{
    gameSpeed = 50;
    score = 0;
    level = 1;
    blockCount = 0;
    totalBlocks = 0;
    blockRange = CShape::DEFAULT_RANGE;
    // clear(BLACK);
    m_grid->clear();
    newShape();
    // ili9488_fill(0, 0, 320, 32, MEDIUM_BLUE);
    printf("(*) grid cleared\n");
}

void CGame::manageGame()
{
    CShape &shape = m_shape;
    int cycles = m_ticks;

    //   printf("cycles: %d gameSpeed:%d\n", cycles, gameSpeed);
    // vTaskDelay(10 / portTICK_PERIOD_MS);

    if ((cycles & 15) == 0)
    {
        if (m_joyState[AIM_UP])
        {
            shape.shift();
            drawShape();
        }
        else if (m_joyState[AIM_LEFT])
        {
            if (canMoveShape(CShape::LEFT))
            {
                eraseShape();
                shape.move(CShape::LEFT);
                drawShape();
            }
        }
        else if (m_joyState[AIM_RIGHT])
        {
            if (canMoveShape(CShape::RIGHT))
            {
                eraseShape();
                shape.move(CShape::RIGHT);
                drawShape();
            }
        }
        else if (m_joyState[AIM_DOWN])
        {
            eraseShape();
            while (canMoveShape(CShape::DOWN))
            {
                shape.move(CShape::DOWN);
            }
            drawShape();
        }
    }
    if ((cycles % gameSpeed) == 0)
    {
        // m_grid->at(1, 10) = 10;
        // printf("at %x\n", m_grid->at(1, 10));

        // printf("here....x=%d, y=%d\n", m_shape.x(), m_shape.y());
        drawShape(false);
        if (m_shape.y() > 0)
        {
            //   printf("grid at %d,%d =  %x\n", m_shape.x(), m_shape.y(), m_grid->at(m_shape.x(), m_shape.y()));
        }
        // printf("...%x %x %x\n", m_shape.tile(0), m_shape.tile(1), m_shape.tile(2));
        //  move shape down
        if (canMoveShape(CShape::DOWN))
        {
            //  printf("........xxxxxxxxxxx.......\n");
            eraseShape();
            shape.move(CShape::DOWN);
            drawShape(false);
        }
        else
        {
            if (shape.y() <= 0)
            {
                initGame();
                cycles = 0;
            }
            else
            {
                uint16_t removedBlocks = managePeers(shape);
                score += removedBlocks;
                /*if (removedBlocks)
                {
                    printf("blockCount %d + removedBlocks:%u = newTotal %d; score: %lu\n",
                           blockCount, removedBlocks, blockCount + removedBlocks, score);
                }*/
                blockCount += removedBlocks;
                totalBlocks += removedBlocks;
                bool levelChanged = false;
                while (blockCount >= blocksPerLevel)
                {
                    blockCount -= blocksPerLevel;
                    score += levelBonus;
                    gameSpeed -= speedOffset;
                    levelChanged = true;
                }
                if (levelChanged)
                {
                    level++;
                    // printf(">> level %d\n", level);
                    if (level % 3 == 0)
                    {
                        blockRange = std::min(blockRange + 1, m_blocks->getSize() - 1);
                    }
                }
                if (removedBlocks)
                {
                    // drawStatus();
                }
                //   vTaskDelay(levelChanged ? 100 : 50 / portTICK_PERIOD_MS);
            }
            newShape();
        }
        drawShape();
    }
}
