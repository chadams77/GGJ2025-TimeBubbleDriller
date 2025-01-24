#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::set;

const char * WINDOW_TITLE = "Time-Bubble Driller";

RenderWindow * window = NULL;

int VP_WIDTH = 800, VP_HEIGHT = 600;
const double PI = atan(1.) + atan(2.) + atan(3.);

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

int main() {

    bool fullscreen = false;

    window = new RenderWindow(VideoMode(800, 600), WINDOW_TITLE);
    window->setMouseCursorVisible(false);

    window->setFramerateLimit(60);    

    double time = 0.;
    bool lMouseLeft = false, lMouseRight = false;
    
    while (window->isOpen()) {
        Event event;
        while (window->pollEvent(event)) {
            if (event.type == Event::Closed) {
                window->close();
            }
            else if (event.type == Event::Resized) {
                VP_WIDTH = window->getSize().x;
                VP_HEIGHT = window->getSize().y;
	            window->setView(View(FloatRect(0.f, 0.f, (float)VP_WIDTH, (float)VP_HEIGHT)));
            }
            else if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Key::F11) {
                    fullscreen = !fullscreen;
                    delete window;
                    window = new RenderWindow(fullscreen ? VideoMode::getDesktopMode() : VideoMode(800, 600), WINDOW_TITLE, fullscreen ? Style::Fullscreen : Style::Default);
                    window->setFramerateLimit(60);
                    VP_WIDTH = window->getSize().x;
                    VP_HEIGHT = window->getSize().y;
	                window->setView(View(FloatRect(0.f, 0.f, (float)VP_WIDTH, (float)VP_HEIGHT)));
                }
            }
        }

        lMouseLeft = Mouse::isButtonPressed(Mouse::Left);
        lMouseRight = Mouse::isButtonPressed(Mouse::Right);

        Vector2i mousePosI = Mouse::getPosition(*window);
        double mouseX, mouseY;
        InvTransform(mousePosI.x, mousePosI.y, mouseX, mouseY);

        window->clear(Color::Black);

        double dt = 1. / 60.;

        window->display();
    }

    delete window;

    return 0;
}