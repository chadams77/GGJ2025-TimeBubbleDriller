#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

using std::cerr;
using std::cout;
using std::endl;

const Vector2f GRAVITY = Vector2f(0., 10.);
const int MAX_PRT = 1024;
const int TER_SIZE = 2048;
const int H_SIZE = 2048;
#define TER_POW 11
#define H_POW 11

class Particle {
public:
    bool active;
    uint32_t clr;
    float m;
    Vector2f lp, p, v;
    Particle * next;
    bool presist;
    float life;
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
             PAL_PINK_3,
             PAL_GOLD_1,
             PAL_GOLD_2,
             PAL_GOLD_3,
             PAL_EMPTY;
    
    Particle * prt;
    Particle ** phash;
    int pind;

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
        int size = 2048 >> level;
        float fx = (float)x / (float)(1 << level);
        float fy = (float)y / (float)(1 << level);
        int ix = (int)floor(fx),
            iy = (int)floor(fy);
        float ax = fx - floorf(fx),
              ay = fy - floorf(fy);
        int v00 = ix >= 0 && iy >= 0 && ix < size && iy < size ? lbfr[ix + (iy << (TER_POW-level))] : 0;
        int v10 = ix >= 0 && iy >= 0 && ix < (size-1) && iy < size ? lbfr[ix + 1 + (iy << (TER_POW-level))] : 0;
        int v01 = ix >= 0 && iy >= 0 && ix < size && iy < (size-1) ? lbfr[ix + ((iy + 1) << (TER_POW-level))] : 0;
        int v11 = ix >= 0 && iy >= 0 && ix < (size-1) && iy < (size-1) ? lbfr[ix + 1 + ((iy + 1) << (TER_POW-level))] : 0;
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
        PAL_PINK_3 = sprites->getPixel(141, 0);
        PAL_GOLD_1 = sprites->getPixel(4 * 32 + 10, 0);
        PAL_GOLD_2 = sprites->getPixel(4 * 32 + 11, 0);
        PAL_GOLD_3 = sprites->getPixel(140, 0);
        PAL_EMPTY = 0;

        uint32_t PAL[] = {
            PAL_EMPTY,
            PAL_GREY_2,
            PAL_GREY_1,
            PAL_BROWN_1,
            PAL_BROWN_2
        };

        int * levels[] = {
            new int[1024 * 1024],
            new int[512 * 512],
            new int[256 * 256],
            new int[128 * 128],
            new int[64 * 64]
        };

        for (int i=0; i<5; i++) {
            size_t count = (2048>>(i+1))*(2048>>(i+1));
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
                if (y < 24) {
                    int r = rand() % (y/4+1);
                    if (r == 0 && v > 0) {
                        bfr[x + (y << TER_POW)] = PAL_GREEN_1;
                    }
                }
                if (y < 16) {
                    int r = rand() % (y/2+1);
                    if (r == 0 && v > 0) {
                        bfr[x + (y << TER_POW)] = PAL_GREEN_2;
                    }
                }
            }
        }

        for (int x=1; x<(TER_SIZE-1); x++) {
            for (int y=1; y<(TER_SIZE-1); y++) {
                if (!bfr[x + (y << TER_POW)] && (bfr[(x+1) + (y << TER_POW)] || bfr[(x-1) + (y << TER_POW)] || bfr[x + ((y+1) << TER_POW)] || bfr[x + ((y-1) << TER_POW)])) {
                    if (!(rand()%50)) {
                        drawSprite(SSprite(sprites, 144, 0, 3, 2), Vector2i(x-1, y));
                    }
                    else if (!(rand()%400)) {
                        drawSprite(SSprite(sprites, 147, 0, 5, 5), Vector2i(x-2, y-2));
                    }
                }
            }
        }

        pind = 0;

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

    float dig(SSprite sprite, Vector2f p, float angle, float & gold, float & crystal) {
        const int ca = (int)(cos(-angle)*10000.f), sa = (int)(sin(-angle)*10000.f);
        const int r = (int)ceilf(sqrtf((sprite.size.x, sprite.size.y) * max(sprite.size.x, sprite.size.y) / 2));

        const int cx = sprite.size.x / 2,
                  cy = sprite.size.y / 2;
        
        float totalCnt = 1.f, countEmpty = 0.f;

        for (int y = -r; y <= r; y++) {
            const int wy = p.y + y;
            if (wy < 0 || wy >= TER_SIZE) {
                continue;
            }
            uint32_t * wptr = bfr + wy * TER_SIZE;
            for (int x = -r; x <= r; x++) {
                const int wx = x + p.x;
                if (wx < 0 || wx >= TER_SIZE) {
                    continue;
                }
                const int rx = (x * ca - y * sa) / 10000 + sprite.pos.x + cx,
                          ry = (y * ca + x * sa) / 10000 + sprite.pos.y + cy;
                if (rx >= sprite.pos.x && ry >= sprite.pos.y && rx < (sprite.pos.x + sprite.size.x) && ry < (sprite.pos.y + sprite.size.y)) {
                    const uint32_t v = sprite.sheet->bfr[rx + ry * sprite.sheet->size.x];
                    if (v > 0) {
                        totalCnt += 1.f;
                        const uint32_t gv = wptr[wx];
                        if (gv > 0) {
                            bool spec = false;
                            if (gv == PAL_GREY_2) {
                                countEmpty += 0.f;
                                if (!(rand()&31)) {
                                    wptr[wx] = 0;
                                }
                            }
                            else if (gv == PAL_GREY_1) {
                                countEmpty += 0.1f;
                                if (!(rand()&15)) {
                                    wptr[wx] = 0;
                                }
                            }
                            else if (gv == PAL_BROWN_2) {
                                countEmpty += 0.05f;
                                if (!(rand()&7)) {
                                    wptr[wx] = 0;
                                }
                            }
                            else if (gv == PAL_BROWN_1) {
                                countEmpty += 0.05f;
                                if (!(rand()&3)) {
                                    wptr[wx] = 0;
                                }
                            }
                            else if (gv == PAL_GOLD_1 || gv == PAL_GOLD_2 || gv == PAL_GOLD_3) {
                                countEmpty += 0.25f;
                                if (!(rand()&3)) {
                                    wptr[wx] = 0;
                                    gold += 1.f;
                                    spec = true;
                                }
                            }
                            else if (gv == PAL_PINK_1 || gv == PAL_PINK_2 || gv == PAL_PINK_3) {
                                countEmpty += 0.25f;
                                if (!(rand()&3)) {
                                    wptr[wx] = 0;
                                    crystal += 1.f;
                                    spec = true;
                                }
                            }
                            else if (gv == PAL_GREEN_1 || gv == PAL_GREEN_2) {
                                countEmpty += 0.25f;
                                if (!(rand()&3)) {
                                    wptr[wx] = 0;
                                }
                            }
                            if (wptr[wx] == 0 && !spec) {
                                prt[pind].p = Vector2f((float)wx, (float)wy);
                                prt[pind].lp = prt[pind].p - Vector2f((float)((rand()&63)-31), (float)(-(rand()&63))) * 0.01f;
                                prt[pind].v = Vector2f(0.f, 0.f);
                                prt[pind].clr = gv;
                                prt[pind].active = true;
                                prt[pind].m = 1.0;
                                prt[pind].presist = false;
                                prt[pind].life = 2.0;
                                pind = (pind + 1) % MAX_PRT;
                            }
                        }
                        else {
                            countEmpty += 1.0f;
                        }
                    }
                }
            }
        }

        return countEmpty / max(totalCnt, 1.f);
    }

    void updatePHash () {
        memset(phash, 0, sizeof(Particle*)*H_SIZE*H_SIZE);
        for (int i=0; i<MAX_PRT; i++) {
            if (prt[i].active) {
                int ix = ((int)floor(prt[i].p.x)), iy = ((int)floor(prt[i].p.y));
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
                prt[i].v = prt[i].p - prt[i].lp;
                prt[i].lp = prt[i].p;
                prt[i].p += GRAVITY * dt + prt[i].v * 0.75f;
                int ix = ((int)floor(prt[i].p.x)), iy = ((int)floor(prt[i].p.y));
                if (ix < 0 || iy < 0 || ix >= TER_SIZE || iy >= TER_SIZE || prt[i].life < 0.f) {
                    prt[i].active = false;
                    continue;
                }
            }
        }

        for (int k=0; k<8; k++) {
            updatePHash();
            
            for (int i=0; i<MAX_PRT; i++) {
                if (prt[i].active) {
                    int ix = ((int)floor(prt[i].p.x)), iy = ((int)floor(prt[i].p.y));
                    for (int ox=-2; ox<=2; ox++) {
                        for (int oy=-2; oy<=2; oy++) {
                            int ix2 = ox + ix, iy2 = oy + iy;
                            if (ix2 >= 0 && iy2 >= 0 && ix2 < H_SIZE && iy2 < H_SIZE) {
                                Particle * ptr = phash[ix2 + (iy2 << H_POW)];
                                while (ptr != NULL) {
                                    if ((prt - ptr) != i) {
                                        Vector2f dir = prt[i].p - ptr->p;
                                        float len = sqrtf(dir.x*dir.x+dir.y*dir.y);
                                        if (len < 1.f) {
                                            float f = (1.f - len) / 2.f;
                                            prt[i].p += dir * (ptr->m / (prt[i].m + ptr->m)) * 0.25f * 0.125f;
                                            ptr->p -= dir * (prt[i].m / (prt[i].m + ptr->m)) * 0.25f * 0.125f;
                                        }
                                    }
                                    ptr = prt->next;
                                }
                            }
                        }
                    }
                }
                if (prt[i].active) {
                    int ix = ((int)floor(prt[i].p.x)), iy = ((int)floor(prt[i].p.y));
                    for (int ox=-2; ox<=2; ox++) {
                        for (int oy=-2; oy<=2; oy++) {
                            int ix2 = ox + ix, iy2 = oy + iy;
                            if (ix2 >= 0 && iy2 >= 0 && ix2 < TER_SIZE && iy2 < TER_SIZE) {
                                if (bfr[ix2 + (iy2 << TER_POW)] > 0) {
                                    Vector2f p2 = Vector2f((float)ix2, (float)iy2);
                                    Vector2f dir = prt[i].p - p2;
                                    float len = sqrtf(dir.x*dir.x+dir.y*dir.y);
                                    if (len < 1.f) {
                                        float f = (1.f - len) / 1.f;
                                        prt[i].p += dir * f * 0.25f;
                                        prt[i].life -= f * dt * 0.5f;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        updatePHash();
    }

    float warpf = 0.f;

    void render (Renderer * renderer, Camera * camera, bool slowMo, Vector2f playerp, float dt) {
        uint32_t * wbfr = renderer->bfr;
        if (slowMo) {
            warpf += (1. - warpf) * 16.f * dt;
        }
        else {
            warpf += (0.f - warpf) * 16.f / 4.f * dt;
        }
        for (int y=0; y<REN_HEIGHT; y++) {
            for (int x=0; x<REN_WIDTH; x++) {
                Vector2f wp = camera->unproject(Vector2f((float)x, (float)y));
                Vector2f dirp = wp - playerp;
                float dist = sqrt(dirp.x*dirp.x + dirp.y*dirp.y) + 0.0001f;
                float f = dist;
                wp = wp * (1.f - warpf) + warpf * (playerp + dirp/dist * (powf(f/64.f, 1.25f)*64.f));
                int tx = (int)roundf(wp.x), ty = (int)roundf(wp.y);
                if (tx >= 0 && ty >= 0 && tx < TER_SIZE && ty < TER_SIZE) {
                    uint32_t clr = bfr[tx + (ty << TER_POW)];
                    if (clr > 0) {
                        *wbfr = clr;
                    }
                    Particle * p = phash[tx + (ty << H_POW)];
                    if (p) {
                        *wbfr = p->clr;
                    }
                }
                wbfr ++;
            }
        }
    }
};