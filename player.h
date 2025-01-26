#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

using std::cerr;
using std::cout;
using std::endl;

class Player {
public:
    Vector2f p, v;
    float a, av;
    float hp;
    float digf;
    float atime;
    float gold;
    float crystal;

    Player(Vector2f _p = Vector2f(0.f, 0.f)) {
        p = _p;
        v = Vector2f(0.f, 0.f);
        a = PI*0.5;
        av = 0.f;
        hp = 100.0f;
        atime = 0.f;
        digf = 0.f;
        gold = 0.f;
        crystal = 10.f;
    }

    void update(Camera * camera, SpriteSheet * sprites, Ground * ground, float dt) {

        float f = ground->dig(SSprite(sprites, 0, 0, 32, 32), p, a, gold, crystal);
        digf += (1.f - f);
        digf -= digf * dt * 4.f;

        v -= v * 4.f * dt;
        v -= v * min(max(digf, 0.f), 1.0f) * 0.25f;
        av -= av * min(max(digf, 0.f), 1.0f) * 0.25f;
        a -= av * 4.f * dt;
        p += v * dt;
        a += av * dt;

        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
            av += dt * 0.3f;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
            av -= dt * 0.3f;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
            v += Vector2f(cosf(a-PI*0.5f), sinf(a-PI*0.5f)) * 120.f * dt;
        }

        camera->lookAtSmooth(p, 4.0f);
        camera->setShake(digf);

        atime += dt;
    }

    void render(Renderer * renderer, Camera * camera, SpriteSheet * sprites) {
        SSprite sprite = (int)(atime * 1.5f) % 2 ? SSprite(sprites, 0, 0, 32, 32) : SSprite(sprites, 32, 0, 32, 32);
        renderer->drawSpriteRot(sprite, p, a, camera);
    }
};