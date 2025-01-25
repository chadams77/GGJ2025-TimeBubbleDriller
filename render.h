#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

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

    void render(RenderWindow * window, float dt) {
        texture.update((Uint8*)bfr);
        sprite.setPosition(Vector2f(0., 0.));
        AutoTransform(sprite);
        window->draw(sprite);
    }
};