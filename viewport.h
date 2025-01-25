#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

int VP_WIDTH = 800, VP_HEIGHT = 600;

void AutoTransform ( Sprite * sprite ) {
    double aspectA = (double)VP_WIDTH / (double)VP_HEIGHT;
    double aspect = 320. / 240.;
    double scale = aspectA < aspect ? ((double)VP_WIDTH / 320.) : ((double)VP_HEIGHT / 240.);
    sprite->setScale(scale, scale);
    Vector2f pos = sprite->getPosition();
    pos.x -= 320. * 0.5;
    pos.y -= 240. * 0.5;
    pos.x *= scale;
    pos.y *= scale;
    pos.x += (double)VP_WIDTH * 0.5;
    pos.y += (double)VP_HEIGHT * 0.5;
    sprite->setPosition(pos);
}

void AutoTransform ( Sprite & sprite ) {
    AutoTransform(&sprite);
}

void InvTransform ( int sx, int sy, double & rx, double & ry ) {
    double aspectA = (double)VP_WIDTH / (double)VP_HEIGHT;
    double aspect = 320. / 240.;
    double scale = aspectA < aspect ? ((double)VP_WIDTH / 320.) : ((double)VP_HEIGHT / 240.);
        
    rx = (double)sx;
    ry = (double)sy;
    rx -= (double)VP_WIDTH * 0.5;
    ry -= (double)VP_HEIGHT * 0.5;
    rx /= scale;
    ry /= scale;
    rx += 320. * 0.5;
    ry += 240. * 0.5;
}

Vector2i GetViewport() {
    return Vector2i(VP_WIDTH, VP_WIDTH);
}

void OnResize (RenderWindow * window) {
    VP_WIDTH = window->getSize().x;
    VP_HEIGHT = window->getSize().y;
    window->setView(View(FloatRect(0.f, 0.f, (float)VP_WIDTH, (float)VP_HEIGHT)));
}