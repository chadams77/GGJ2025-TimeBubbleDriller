#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "viewport.h"
#include "render.h"
#include "ground.h"

using namespace sf;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::set;

const char * WINDOW_TITLE = "Time-Bubble Driller";
const double PI = 3.1415926535897932;

RenderWindow * window = NULL;
Renderer * renderer = NULL;
SpriteSheet * sprites = NULL;
Camera * camera = NULL;
Camera * uiCamera = NULL;
Ground * ground = NULL;

int main() {

    bool fullscreen = false;

    window = new RenderWindow(VideoMode(800, 600), WINDOW_TITLE);
    window->setMouseCursorVisible(false);
    window->setFramerateLimit(60);    

    sprites = new SpriteSheet("images/sprites.png");

    ground = new Ground();
    ground->initLevel(sprites);

    camera = new Camera(Vector2f(0., 0.));
    uiCamera = new Camera();

    renderer = new Renderer();

    double time = 0.;
    bool lMouseLeft = false, lMouseRight = false;

    camera->lookAtSmooth(Vector2f(128., 128.), 0.5f);
    
    while (window->isOpen()) {
        Event event;
        while (window->pollEvent(event)) {
            if (event.type == Event::Closed) {
                window->close();
            }
            else if (event.type == Event::Resized) {
                OnResize(window);
            }
            else if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Key::F11) {
                    fullscreen = !fullscreen;
                    delete window;
                    window = new RenderWindow(
                        fullscreen ? VideoMode::getDesktopMode() : VideoMode(800, 600),
                        WINDOW_TITLE,
                        fullscreen ? Style::Fullscreen : Style::Default
                    );
                    window->setFramerateLimit(60);
                    OnResize(window);
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
        time += dt;

        camera->update(dt);

        ground->update(camera, dt);

        renderer->clear();

        ground->render(renderer, camera);

        renderer->drawSpriteRot(SSprite(sprites, 0, 0, 32, 32), Vector2f(64., 64.) + Vector2f(cos(time*2.), sin(time*2.)) * 12.f, time, camera);
        renderer->drawSprite(SSprite(sprites, 0, 0, 32, 32), Vector2f(64., 64.), camera);
        renderer->drawSpriteRot(SSprite(sprites, 0, 0, 32, 32), Vector2f(64., 64.) + Vector2f(cos(-time*2.), sin(-time*2.)) * 12.f, -time * 4., camera);

        renderer->render(window, dt);

        window->display();
    }

    delete ground;
    delete sprites;
    delete renderer;
    delete camera;
    delete uiCamera;
    delete window;

    return 0;
}