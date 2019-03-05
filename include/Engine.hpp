#pragma once

#include <cassert>
#include <functional>
#include <iostream>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Platform/GLContext.h>
#include <Magnum/Shaders/VertexColor.h>

#include <SDL2/SDL.h>

#define WITH_FUNC(TYPE, NAME, VARNAME) TYPE& with##NAME(auto v) {VARNAME = v; return *this;}

class Engine {
public:
  Engine() : ctx_{Magnum::NoCreate} {
    using namespace Magnum;
    SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_SENSOR));

    window_ = SDL_CreateWindow("Glenny", 0, 0, width_, height_, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN);
    SDL_GL_MakeCurrent(window_, SDL_GL_CreateContext(window_));

    
    ctx_.create();
  }

  using SDLEventHandler = std::function<bool(SDL_Event&)>;
  using KeyHandler = std::function<void(const SDL_Keysym&, bool)>;
  using MouseHandler = std::function<void(const SDL_MouseMotionEvent&)>;
  using MouseButtonHandler = std::function<void(int, bool)>;
  using QuitHandler = std::function<void()>;

  inline Engine& addSDLHandler(SDLEventHandler&& handler) {
    handlers_.sdl_.push_back(handler);
    return *this;
  }

  inline Engine& addKeyHandler(KeyHandler&& handler) {
    handlers_.key_.push_back(handler);
    return *this;
  }

  inline Engine& addMouseHandler(MouseHandler&& handler) {
    handlers_.mouse_.push_back(handler);
    return *this;
  }

  inline Engine& addMouseButtonHandler(MouseButtonHandler&& handler) {
    handlers_.mouseBtn_.push_back(handler);
    return *this;
  }

  inline Engine& addQuitHandler(QuitHandler&& handler) {
    handlers_.quit_.push_back(handler);
    return *this;
  }

  inline  Engine& handleEvents() {
    SDL_Event ev;
    evLoop: while(SDL_PollEvent(&ev)) {
      for(auto handler : handlers_.sdl_)
        if(handler(ev))
          goto evLoop; // May the great Bjorne forgive me
      

      int x, y;
      switch(ev.type) {
        case SDL_QUIT:
          for(auto handler : handlers_.quit_) handler();
          break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
          for(auto handler : handlers_.key_) handler(ev.key.keysym, ev.type == SDL_KEYDOWN);
          break;

        case SDL_MOUSEMOTION:
          for(auto handler : handlers_.mouse_) handler(ev.motion);
          break;

          default:
          break;
      }
    }

    return *this;
  }

  inline Engine& handleRender() {
    SDL_GL_SwapWindow(window_);
    return *this;
  }

private:
  struct {
    std::vector<SDLEventHandler> sdl_;
    std::vector<KeyHandler> key_;
    std::vector<MouseHandler> mouse_;
    std::vector<MouseButtonHandler> mouseBtn_;
    std::vector<QuitHandler> quit_;
  } handlers_;

  SDL_Window* window_;
  Magnum::GL::DefaultFramebuffer fb_;
  Magnum::Platform::GLContext ctx_;
  int width_ = 1360, height_ = 768;
};