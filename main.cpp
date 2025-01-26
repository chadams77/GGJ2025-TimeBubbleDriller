#include <iostream>
#include <fstream>
#include <sstream>
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
using std::ostringstream;

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

        bool slowMo = player->update(camera, sprites, ground, dt);
        if (slowMo) {
            dt *= 0.25f;
        }

        camera->update(dt);
        camera->bound(0.f, 0.f, (float)(TER_SIZE-1), (float)(TER_SIZE-1));

        ground->update(camera, dt);
        lava->update(dt);

        renderer->clear();

        renderer->drawSprite(SSprite(sprites, 192, 0, 320, 240), Vector2i(0, 0));

        ground->render(renderer, camera, slowMo, player->p, dt);
        player->render(renderer, camera, sprites);
        lava->render(renderer, camera);

        int money = (int)ceil(player->gold * 1000.f);
        int crystal = max(0, min(45, (int)ceil(player->crystal / 25.f * 45.f)));

        ostringstream oss;
        oss << "$" << round(money);

        renderer->drawSprite(SSprite(sprites, 4, 59, 47, 5), Vector2i(4, 4));
        renderer->drawSprite(SSprite(sprites, 5, 54, crystal, 3), Vector2i(5, 5));

        for (size_t i=0; i<oss.str().size(); i++) {
            char ch = oss.str()[i];
            if (ch == '$') {
                renderer->drawSprite(SSprite(sprites, 0, 32, 16, 16), Vector2i(4, 10));
            }
            else {
                renderer->drawSprite(SSprite(sprites, 16+16*(ch-'0'), 32, 16, 16), Vector2i(4+i*12, 10));
            }
        }

        renderer->drawSprite(SSprite(sprites, 512, 0, 16, 240), Vector2i(320-16, 0));
        int lv = max(0, min(240, (int)((lava->lavaY / (2048.f+512.f)) * 240.f)));
        renderer->drawSprite(SSprite(sprites, 512+16, lv, 16, 240 - lv), Vector2i(320-16, lv));

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