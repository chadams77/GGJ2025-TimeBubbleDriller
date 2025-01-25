#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using std::cerr;
using std::cout;
using std::endl;

const int REN_WIDTH = 320, REN_HEIGHT = 240;

class Camera {
private:
    Vector2f hsz;
public:
    Vector2f p, toP;
    float lookSpeed = 1.f;

    Camera(Vector2f _p) {
        hsz = Vector2f((float)REN_WIDTH, (float)REN_HEIGHT) * 0.5f;
        lookAt(_p);
    }
    Camera() { // UI Camera
        hsz = Vector2f((float)REN_WIDTH, (float)REN_HEIGHT) * 0.5f;
        lookAt(hsz);
    }

    Vector2f project(Vector2f wp) {
        return wp - p + hsz;
    }

    Vector2f unproject(Vector2f sp) {
        return sp - hsz + p;
    }

    Vector2f unproject_real(Vector2i real) {
        double spx, spy;
        InvTransform(real.x, real.y, spx, spy);
        return Vector2f(spx, spy) - hsz + p;
    }

    void lookAt(Vector2f _p) {
        p = _p;
        toP = _p;
    }

    void lookAtSmooth(Vector2f _p, float speed = 4.f) {
        toP = _p;
        lookSpeed = speed;
    }

    void update(float dt) {
        p += (toP - p) * dt * lookSpeed;
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
            uint32_t const * rptr = sprite.sheet->bfr + (sprite.pos.y + y) * sprite.sheet->size.x;
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

    void render(RenderWindow * window, float dt) {
        texture.update((Uint8*)bfr);
        sprite.setPosition(Vector2f(0., 0.));
        AutoTransform(sprite);
        window->draw(sprite);
    }
};