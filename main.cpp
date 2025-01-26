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
#include "lava.h"
#include "player.h"

using namespace sf;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::set;

const char * WINDOW_TITLE = "Time-Bubble Driller";

RenderWindow * window = NULL;
Renderer * renderer = NULL;
SpriteSheet * sprites = NULL;
Camera * camera = NULL;
Camera * uiCamera = NULL;
Ground * ground = NULL;
Lava * lava = NULL;
Player * player = NULL;

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

    lava = new Lava(2048.f + 512.f);

    player = new Player(Vector2f(1023.f, 2047.f - 16.f));
    player->a -= PI * 0.5f;

    renderer = new Renderer();

    double time = 0.;
    bool lMouseLeft = false, lMouseRight = false;
   
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

        player->update(camera, sprites, ground, dt);

        camera->update(dt);
        camera->bound(0.f, 0.f, (float)(TER_SIZE-1), (float)(TER_SIZE-1));

        ground->update(camera, dt);
        lava->update(dt);

        renderer->clear();

        renderer->drawSprite(SSprite(sprites, 192, 0, 320, 240), Vector2i(0, 0));

        ground->render(renderer, camera);
        player->render(renderer, camera, sprites);
        lava->render(renderer, camera);

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