#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

using std::cerr;
using std::cout;
using std::endl;

const int TER_SIZE = 1024;
#define TER_POW 10

class Ground {
public:
    uint32_t * bfr = NULL;

    uint32_t PAL_BROWN_1,
             PAL_BROWN_2,
             PAL_BLUE_1,
             PAL_BLUE_2,
             PAL_GREY_1,
             PAL_GREY_2,
             PAL_GREEN_1,
             PAL_GREEN_2,
             PAL_PINK_1,
             PAL_PINK_2,
             PAL_GOLD_1,
             PAL_GOLD_2,
             PAL_EMPTY;

    Ground() {
        bfr = new uint32_t[TER_SIZE * TER_SIZE];
        memset(bfr, 0, sizeof(uint32_t)*TER_SIZE*TER_SIZE);
    }
    ~Ground() {
        delete bfr;
    }

    int lget (int * lbfr, int x, int y, int level) {
        int size = 1024 >> level;
        float fx = (float)x / (float)(1 << level);
        float fy = (float)y / (float)(1 << level);
        int ix = (int)floor(fx),
            iy = (int)floor(fy);
        float ax = fx - floorf(fx),
              ay = fy - floorf(fy);
        int v00 = ix >= 0 && iy >= 0 && ix < size && iy < size ? lbfr[ix + (iy << (10-level))] : 0;
        int v10 = ix >= 0 && iy >= 0 && ix < (size-1) && iy < size ? lbfr[ix + 1 + (iy << (10-level))] : 0;
        int v01 = ix >= 0 && iy >= 0 && ix < size && iy < (size-1) ? lbfr[ix + ((iy + 1) << (10-level))] : 0;
        int v11 = ix >= 0 && iy >= 0 && ix < (size-1) && iy < (size-1) ? lbfr[ix + 1 + ((iy + 1) << (10-level))] : 0;
        float a = v00 * (1 - ax) + v10 * ax;
        float b = v01 * (1 - ax) + v11 * ax;
        return (int)(a * (1 - ay) + b * ay);
    }

    void initLevel (SpriteSheet * sprites) {
        srand(time(0));

        PAL_BROWN_1 = sprites->getPixel(4 * 32 + 0, 0);
        PAL_BROWN_2 = sprites->getPixel(4 * 32 + 1, 0);
        PAL_BLUE_1 = sprites->getPixel(4 * 32 + 2, 0);
        PAL_BLUE_2 = sprites->getPixel(4 * 32 + 3, 0);
        PAL_GREY_1 = sprites->getPixel(4 * 32 + 4, 0);
        PAL_GREY_2 = sprites->getPixel(4 * 32 + 5, 0);
        PAL_GREEN_1 = sprites->getPixel(4 * 32 + 6, 0);
        PAL_GREEN_2 = sprites->getPixel(4 * 32 + 7, 0);
        PAL_PINK_1 = sprites->getPixel(4 * 32 + 8, 0);
        PAL_PINK_2 = sprites->getPixel(4 * 32 + 9, 0);
        PAL_GOLD_1 = sprites->getPixel(4 * 32 + 10, 0);
        PAL_GOLD_2 = sprites->getPixel(4 * 32 + 11, 0);
        PAL_EMPTY = 0;

        uint32_t PAL[] = {
            PAL_EMPTY,
            PAL_GREY_2,
            PAL_GREY_1,
            PAL_BROWN_1,
            PAL_BROWN_2
        };

        int * levels[] = {
            new int[512 * 512],
            new int[256 * 256],
            new int[128 * 128],
            new int[64 * 64],
            new int[32 * 32]
        };

        for (int i=0; i<5; i++) {
            size_t count = (1024>>(i+1))*(1024>>(i+1));
            for (size_t j=0; j<count; j++) {
                levels[i][j] = rand() % 1000;
            }
        }

        for (int x=0; x<TER_SIZE; x++) {
            for (int y=0; y<TER_SIZE; y++) {
                int v = 0;
                for (int l=0; l<5; l++) {
                    v += lget(levels[l], x, y, l+1) >> (5 - l);
                }
                v = (v * v * v) / 1000000;
                v = max(0, min(4, v * 5 / 400));
                bfr[x + (y << TER_POW)] = PAL[v];
            }
        }

        for (int i=0; i<5; i++) {
            delete levels[i];
        }
    }

    void drawSprite(SSprite sprite, Vector2i p) {
        Vector2i bottom = p + sprite.size;
        if (bottom.x < 0 || bottom.y < 0 || p.x >= TER_SIZE || p.y >= TER_SIZE) {
            return;
        }
        for (int y = 0; y < sprite.size.y; y++) {
            if ((y+p.y) >= TER_SIZE) {
                return;
            }
            if ((y+p.y) < 0) {
                continue;
            }
            uint32_t const * rptr = sprite.sheet->bfr + (sprite.pos.y + y) * sprite.sheet->size.x + sprite.pos.x;
            uint32_t * wptr = bfr + ((y + p.y) << TER_POW) + p.x;
            for (int x = 0; x < sprite.size.x; x++) {
                if ((x+p.x) >= TER_SIZE) {
                    break;
                }
                if ((x+p.x) < 0) {
                    continue;
                }
                uint32_t v = rptr[x];
                if (v > 0) {
                    wptr[x] = v;
                }
            }
        }
    }

    void render (Renderer * renderer, Camera * camera) {
        uint32_t * wbfr = renderer->bfr;
        for (int y=0; y<REN_HEIGHT; y++) {
            for (int x=0; x<REN_WIDTH; x++) {
                Vector2f wp = camera->unproject(Vector2f((float)x, (float)y));
                int tx = (int)roundf(wp.x), ty = (int)roundf(wp.y);
                if (tx >= 0 && ty >= 0 && tx < TER_SIZE && ty < TER_SIZE) {
                    *wbfr = bfr[tx + (ty << TER_POW)];
                }
                wbfr ++;
            }
        }
    }
};