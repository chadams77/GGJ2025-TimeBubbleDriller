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
    float ngold, gold;
    float crystal;
    bool win, loose;
    float wltime;

    Player(Vector2f _p = Vector2f(0.f, 0.f)) {
        p = _p;
        v = Vector2f(0.f, 0.f);
        a = PI*0.5;
        av = 0.f;
        hp = 100.0f;
        atime = 0.f;
        digf = 0.f;
        gold = 0.f;
        ngold = 0.f;
        crystal = 10.f;
        win = loose = false;
        wltime = 0.f;
    }

    bool update(Camera * camera, SpriteSheet * sprites, Ground * ground, float lavaY, float dt) {

        bool slowMo = false;
        if (Keyboard::isKeyPressed(Keyboard::Key::Space) && crystal > 0.f) {
            crystal -= dt * 6.f;
            slowMo = true;
            if (crystal < 0.f) {
                crystal = 0.f;
            }
        }

        gold += (ngold - gold) * dt * 2.5f;

        if (p.y > 0.f) {
            float f = ground->dig(SSprite(sprites, 0, 0, 32, 32), p, a, ngold, crystal);
            digf += (1.f - f);
        }
        else if (!loose) {
            if (!win) {
                ngold *= 2.f;
                playSound(SFX_WIN, 1., 0.5);
            }
            win = true;
        }

        if ((p.y > lavaY || p.y > 2047.f || p.x < 0.f || p.x > 2047.f) && !win) {
            if (!loose) {
                playSound(SFX_DIE, 1., 0.5);
            }
            loose = true;
        }

        if (loose) {
            digf += (8.f - digf) * dt * 8.f;
        }

        digf -= digf * dt * 4.f;

        if (crystal > 45.f) {
            crystal = 45.f;
        }

        if (!win && !loose) {

            v -= v * 2.f * dt;
            v -= v * min(max(digf, 0.f), 1.0f) * 0.25f;
            av -= av * min(max(digf, 0.f), 1.0f) * 0.25f;
            a -= av * 4.f * dt;
            p += 2.f * v * dt;
            a += av * dt;

            if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                av += dt * 10.f * (slowMo ? 2.f : 1.f);
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                av -= dt * 10.f * (slowMo ? 2.f : 1.f);
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                v += Vector2f(cosf(a-PI*0.5f), sinf(a-PI*0.5f)) * 240.f * dt * (slowMo ? 2.f : 1.f);
            }

        }
        else if (loose) {
            p.y += 8.f * dt;
        }
        else if (win) {
            p.y -= 8.f * dt;
        }

        camera->lookAtSmooth(p, 4.0f);
        camera->setShake(digf * 0.5f);

        atime += dt;

        return slowMo;
    }

    void render(Renderer * renderer, Camera * camera, SpriteSheet * sprites) {
        SSprite sprite = (int)(atime * 1.5f) % 2 ? SSprite(sprites, 0, 0, 32, 32) : SSprite(sprites, 32, 0, 32, 32);
        renderer->drawSpriteRot(sprite, p, a, camera);
    }
};