#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

using std::cerr;
using std::cout;
using std::endl;

class Lava {
public:
    float lavaY;
    float atime;
    
    Lava(float y) {
        lavaY = y;
    }

    void update(float dt) {
        lavaY -= dt * 8.533f;
        atime += dt;
    }

    void render(Renderer * renderer, Camera * camera) {
        uint32_t * wbfr = renderer->bfr;
        for (int y=0; y<REN_HEIGHT; y++) {
            for (int x=0; x<REN_WIDTH; x++) {
                Vector2f wp = camera->unproject(Vector2f((float)x, (float)y));
                int tx = (int)roundf(wp.x), ty = (int)roundf(wp.y);
                if (tx >= 0 && ty >= 0 && tx < TER_SIZE && ty < TER_SIZE) {
                    if ((float)ty > (lavaY + powf(sinf(atime*6.f + (float)tx / 4.f) + 1.f, 0.33f) * 4.f)) {
                        *wbfr = 0xe35100ff;
                    }
                    if ((float)ty > (lavaY + powf(sinf(-atime*4.f + (float)tx / 4.f) + 1.f, 0.33f) * 4.f)) {
                        *wbfr = 0xb21030ff;
                    }
                }
                wbfr ++;
            }
        }
    }
};