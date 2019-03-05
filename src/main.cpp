#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>

#include <Engine.hpp>

const int width = 1360, height = 768;

using namespace std::chrono_literals;
using namespace Magnum;
using namespace Magnum::Math::Literals;

int main(int argc, char** argv) {
    Engine eng;
    

    bool running = true;
    eng.addKeyHandler([&](const SDL_Keysym& key, bool down) {
        if(key.scancode == SDL_SCANCODE_ESCAPE)
            running = false;
    });
    while (running) {
        eng
            .handleEvents()
            .handleRender();
    }
    

    return 0;
}