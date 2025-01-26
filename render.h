#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using std::cerr;
using std::cout;
using std::endl;

const double PI = 3.1415926535897932;
const int REN_WIDTH = 320, REN_HEIGHT = 240;

static float max ( float a, float b ) {
    return a > b ? a : b;
}
static float min ( float a, float b ) {
    return a < b ? a : b;
}
static int max ( int a, int b ) {
    return a > b ? a : b;
}
static int min ( int a, int b ) {
    return a < b ? a : b;
}
static double max ( double a, double b ) {
    return a > b ? a : b;
}
static double min ( double a, double b ) {
    return a < b ? a : b;
}

class Camera {
private:
    Vector2f hsz;
public:
    Vector2f p, toP, off;
    float lookSpeed = 1.f;
    float shake = 0.f;

    Camera(Vector2f _p) {
        hsz = Vector2f((float)REN_WIDTH, (float)REN_HEIGHT) * 0.5f;
        lookAt(_p);
    }
    Camera() { // UI Camera
        hsz = Vector2f((float)REN_WIDTH, (float)REN_HEIGHT) * 0.5f;
        lookAt(hsz);
    }

    Vector2f project(Vector2f wp) {
        return wp - (p + off) + hsz;
    }

    Vector2f unproject(Vector2f sp) {
        return sp - hsz + p + off;
    }

    Vector2f unproject_real(Vector2i real) {
        double spx, spy;
        InvTransform(real.x, real.y, spx, spy);
        return Vector2f(spx, spy) - hsz + p + off;
    }

    void lookAt(Vector2f _p) {
        p = _p;
        toP = _p;
    }

    void lookAtSmooth(Vector2f _p, float speed = 4.f) {
        toP = _p;
        lookSpeed = speed;
    }

    void setShake(float s) {
        shake = s;
    }

    void bound(float x1, float y1, float x2, float y2) {
        if (p.x < (x1+320./2.)) {
            p.x = x1+320./2.;
        }
        if (p.y < (y1+240./2.)) {
            p.y = y1+240./2.;
        }
        if (p.x > (x2-320./2.)) {
            p.x = x2-320./2.;
        }
        if (p.y > (y2-240./2.)) {
            p.y = y2-240./2.;
        }
    }

    void update(float dt) {
        p += (toP - p) * dt * lookSpeed;
        off = Vector2f((float)((rand()&63)-31), (float)((rand()&63)-31)) * shake * 0.025f;
        shake = 0.;
    }
};

class SpriteSheet {
private:
    Texture texture;
    Image image;
public:
    uint32_t const * bfr;
    Vector2i size;

    SpriteSheet(const char * filename) {
        if (!texture.loadFromFile(filename)) {
            cerr << "Sprite sheet not found: " << filename << endl;
            exit(0);
        }

        image = texture.copyToImage();
        bfr = (uint32_t const *)image.getPixelsPtr();
        size = Vector2i(image.getSize().x, image.getSize().y);
    }
    ~SpriteSheet() {
    }

    uint32_t getPixel(int x, int y) {
        if (x<0 || y<0 || x>=size.x || y>=size.y) {
            return 0;
        }
        return bfr[x + y * size.x];
    }
};

class SSprite {
public:
    SpriteSheet * sheet;
    Vector2i pos, size;
    SSprite(SpriteSheet * _sheet, int x, int y, int w = 16, int h = 16) {
        sheet = _sheet;
        pos.x = x; pos.y = y;
        size.x = w; size.y = h;
    }
    SSprite(const SSprite & b) {
        sheet = b.sheet;
        pos = b.pos;
        size = b.size;
    }
};

uint32_t blend ( uint32_t a, uint32_t b, float alpha ) {
    int ar = (a >> 24u) & 0xFF;
    int ag = (a >> 16u) & 0xFF;
    int ab = (a >> 8u) & 0xFF;
    int aa = (a >> 0u) & 0xFF;
    int br = (b >> 24u) & 0xFF;
    int bg = (b >> 16u) & 0xFF;
    int bb = (b >> 8u) & 0xFF;
    int ba = (b >> 0u) & 0xFF;
    int t = (int)floor(alpha * 1023.);
    int rr = max(0, min(255, (br * t + ar * (1023 - t)) >> 10));
    int rg = max(0, min(255, (bg * t + ag * (1023 - t)) >> 10));
    int rb = max(0, min(255, (bb * t + ab * (1023 - t)) >> 10));
    int ra = max(0, min(255, (ba * t + aa * (1023 - t)) >> 10));
    return (((uint32_t)rr) << 24u) | (((uint32_t)rg) << 16u) | (((uint32_t)rb) << 8u) | ((uint32_t)ra);
}

class Renderer {
public:
    Texture texture;
    Sprite sprite;
    uint32_t * bfr = NULL;

    Renderer() {
        texture.create(REN_WIDTH, REN_HEIGHT);
        bfr = new uint32_t[REN_WIDTH * REN_HEIGHT];
        texture.setSmooth(false);
        sprite.setTexture(texture);
    }

    ~Renderer() {
        delete bfr;
    }

    void clear(uint32_t clr = 0x000000FF) {
        uint32_t * ptr = bfr, * endPtr = bfr + (REN_WIDTH * REN_HEIGHT);
        while (ptr != endPtr) {
            *ptr = clr;
            ptr++;
        }
    }

    void drawSprite(SSprite sprite, Vector2f wp, Camera * camera) {
        Vector2f p = camera->project(wp);
        drawSprite(sprite, Vector2i(roundf(p.x), roundf(p.y)));
    }

    void drawSprite(SSprite sprite, Vector2i p) {
        Vector2i bottom = p + sprite.size;
        if (bottom.x < 0 || bottom.y < 0 || p.x >= REN_WIDTH || p.y >= REN_HEIGHT) {
            return;
        }
        for (int y = 0; y < sprite.size.y; y++) {
            if ((y+p.y) >= REN_HEIGHT) {
                return;
            }
            if ((y+p.y) < 0) {
                continue;
            }
            uint32_t const * rptr = sprite.sheet->bfr + (sprite.pos.y + y) * sprite.sheet->size.x + sprite.pos.x;
            uint32_t * wptr = bfr + (y + p.y) * REN_WIDTH + p.x;
            for (int x = 0; x < sprite.size.x; x++) {
                if ((x+p.x) >= REN_WIDTH) {
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

    void drawSpriteFade(SSprite sprite, Vector2f wp, Camera * camera, float alpha) {
        Vector2f p = camera->project(wp);
        drawSpriteFade(sprite, Vector2i(roundf(p.x), roundf(p.y)), alpha);
    }

    void drawSpriteFade(SSprite sprite, Vector2i p, float alpha) {
        Vector2i bottom = p + sprite.size;
        if (bottom.x < 0 || bottom.y < 0 || p.x >= REN_WIDTH || p.y >= REN_HEIGHT) {
            return;
        }
        for (int y = 0; y < sprite.size.y; y++) {
            if ((y+p.y) >= REN_HEIGHT) {
                return;
            }
            if ((y+p.y) < 0) {
                continue;
            }
            uint32_t const * rptr = sprite.sheet->bfr + (sprite.pos.y + y) * sprite.sheet->size.x + sprite.pos.x;
            uint32_t * wptr = bfr + (y + p.y) * REN_WIDTH + p.x;
            for (int x = 0; x < sprite.size.x; x++) {
                if ((x+p.x) >= REN_WIDTH) {
                    break;
                }
                if ((x+p.x) < 0) {
                    continue;
                }
                uint32_t v = rptr[x];
                if (v > 0) {
                    const uint32_t ov = wptr[x];
                    wptr[x] = blend(ov, v, alpha);
                }
            }
        }
    }

    void drawSpriteRot(SSprite sprite, Vector2f wp, float angle, Camera * camera) {
        Vector2f p = camera->project(wp);
        drawSpriteRot(sprite, Vector2i(roundf(p.x), roundf(p.y)), angle);
    }

    void drawSpriteRot(SSprite sprite, Vector2i p, float angle) {
        const int ca = (int)(cos(-angle)*10000.f), sa = (int)(sin(-angle)*10000.f);
        const int r = (int)ceilf(sqrtf((sprite.size.x, sprite.size.y) * max(sprite.size.x, sprite.size.y) / 2));

        const int cx = sprite.size.x / 2,
                  cy = sprite.size.y / 2;

        for (int y = -r; y <= r; y++) {
            const int wy = p.y + y;
            if (wy < 0 || wy >= REN_HEIGHT) {
                continue;
            }
            uint32_t * wptr = bfr + wy * REN_WIDTH;
            for (int x = -r; x <= r; x++) {
                const int wx = x + p.x;
                if (wx < 0 || wx >= REN_WIDTH) {
                    continue;
                }
                const int rx = (x * ca - y * sa) / 10000 + sprite.pos.x + cx,
                          ry = (y * ca + x * sa) / 10000 + sprite.pos.y + cy;
                if (rx >= sprite.pos.x && ry >= sprite.pos.y && rx < (sprite.pos.x + sprite.size.x) && ry < (sprite.pos.y + sprite.size.y)) {
                    const uint32_t v = sprite.sheet->bfr[rx + ry * sprite.sheet->size.x];
                    if (v > 0) {
                        wptr[wx] = v;
                    }
                }
            }
        }
    }

    void render(RenderWindow * window, float dt) {
        texture.update((Uint8*)bfr);
        sprite.setPosition(Vector2f(0., 0.));
        AutoTransform(sprite);
        window->draw(sprite);
    }
};