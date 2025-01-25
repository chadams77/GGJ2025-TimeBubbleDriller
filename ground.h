#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

using std::cerr;
using std::cout;
using std::endl;

const Vector2f GRAVITY = Vector2f(0., 10.);
const int MAX_PRT = 16384;
const int TER_SIZE = 1024;
const int H_SIZE = 512;
#define TER_POW 10
#define H_POW 9

class Particle {
public:
    bool active;
    uint32_t clr;
    float m;
    Vector2f p, v;
    Particle * next;
};

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
    
    Particle * prt;
    Particle ** phash;

    Ground() {
        bfr = new uint32_t[TER_SIZE * TER_SIZE];
        memset(bfr, 0, sizeof(uint32_t)*TER_SIZE*TER_SIZE);
        prt = new Particle[MAX_PRT];
        memset(prt, 0, sizeof(Particle)*MAX_PRT);
        phash = new Particle*[H_SIZE*H_SIZE];
        memset(phash, 0, sizeof(Particle*)*H_SIZE*H_SIZE);
    }
    ~Ground() {
        delete bfr;
        delete prt;
        delete phash;
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
                for (int l=1; l<5; l++) {
                    v += lget(levels[l], x, y, l+1) >> (5 - l);
                }
                v = (v * v * v) / 1000000;
                v = max(0, min(4, v * 5 / 200));
                bfr[x + (y << TER_POW)] = PAL[v];
            }
        }

        int pind = 0;
        for (int k=0; k<32; k++) {
            updatePHash();
            for (int x=0; x<TER_SIZE; x+=2) {
                for (int y=0; y<(TER_SIZE-2); y+=2) {
                    int off = x + (y << TER_POW);
                    int offDown = x + ((y+2) << TER_POW);
                    int hoff = (x>>1) + ((y>>1) << H_POW);
                    int hoffDown = (x>>1) + (((y+2)>>1) << H_POW);
                    int cHere = (bfr[off] ? 1 : 0) + (phash[hoff] ? 1 : 0);
                    int cDown = (bfr[offDown] ? 1 : 0) + (phash[hoffDown] ? 1 : 0);
                    if ((cHere == 0) && (cDown > 0) && (pind < MAX_PRT)) {
                        prt[pind].active = true;
                        prt[pind].p = Vector2f(((float)x) + 1.f, ((float)y) + 1.f);
                        prt[pind].v = Vector2f((-31.f + (float)(rand()&63)) / 8.f, 0.);
                        int r = rand() & 31;
                        if (r < 27) {
                            prt[pind].clr = (rand() & 1) ? PAL_BLUE_1 : PAL_BLUE_2;
                            prt[pind].m = 0.5f;
                        }
                        else if (r < 30) {
                            prt[pind].clr = (rand() & 1) ? PAL_GOLD_1 : PAL_GOLD_2;
                            prt[pind].m = 3.0f;
                        }
                        else {
                            prt[pind].clr = (rand() & 1) ? PAL_PINK_1 : PAL_PINK_2;
                            prt[pind].m = 5.0f;
                        }
                        prt[pind].next = NULL;
                        pind += 1;
                    }
                }
            }
            if (pind >= MAX_PRT) {
                break;
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

    void updatePHash () {
        memset(phash, 0, sizeof(Particle*)*H_SIZE*H_SIZE);
        for (int i=0; i<MAX_PRT; i++) {
            if (prt[i].active) {
                int ix = ((int)floor(prt[i].p.x)) >> 1, iy = ((int)floor(prt[i].p.y)) >> 1;
                if (ix < 0 || iy < 0 || ix >= H_SIZE || iy >= H_SIZE) {
                    prt[i].active = false;
                    continue;
                }
                int off = ix + (iy << H_POW);
                Particle * nxt = phash[off];
                prt[i].next = nxt;
                phash[off] = prt + i;
            }
        }
    }

    void update (Camera * camera, float dt) {
        for (int i=0; i<MAX_PRT; i++) {
            if (prt[i].active) {
                prt[i].v += GRAVITY * dt;
                prt[i].v -= prt[i].v * dt * 0.25f;
                prt[i].p += prt[i].v * dt;
                int ix = ((int)floor(prt[i].p.x)), iy = ((int)floor(prt[i].p.y));
                if (ix < 0 || iy < 0 || ix >= TER_SIZE || iy >= TER_SIZE) {
                    prt[i].active = false;
                    continue;
                }
                if (bfr[ix + (iy << TER_POW)] > 0) {
                    if (fabsf(prt[i].v.x) > fabsf(prt[i].v.y)) {
                        prt[i].v.x = -prt[i].v.x * 0.75f;
                        prt[i].v.y *= 0.5f;
                    }
                    else {
                        prt[i].v.y = -prt[i].v.y * 0.75f;
                        prt[i].v.x *= 0.5f;
                    }
                }
            }
        }

        updatePHash();

        for (int i=0; i<MAX_PRT; i++) {
            if (prt[i].active) {

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
                    Particle * p = phash[(tx>>1) + ((ty>>1) << H_POW)];
                    if (p) {
                        *wbfr = p->clr;
                    }
                }
                wbfr ++;
            }
        }
    }
};