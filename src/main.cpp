#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include <Engine.hpp>

const int width = 1360, height = 768;

using namespace std::chrono_literals;
using namespace Magnum;
using namespace Magnum::Math::Literals;

int main(int argc, char** argv) {
   Engine eng;

   bool running = true;
   eng.addKeyHandler([&](const SDL_Keysym& key, bool down) {
         if (down && key.scancode == SDL_SCANCODE_ESCAPE)
            running = false;
      })
       .addQuitHandler([&]() { running = false; });

   auto transform = Transform{{0.0, 0.0, -1.0}, {{0.0, 0.0, -1.0}, 0.0}, {1.0, 1.0, 1.0}};

   auto camEnt = eng.createEntity(
       Camera{eng.getPrimaryFramebuffer(), Matrix4::perspectiveProjection(90.0_degf, 16.0 / 9.0, 0.000001, 1000000.0)},
       transform, CachedTransform::fromTransform(transform));

   eng.addUpdateHandler([&](entt::registry<size_t>& reg) {
      auto transform = camEnt.getComponent<Transform>();
      // transform->rot_ += Quaternion::rotation(Deg{0.1f}, Vector3::yAxis());
   });

   auto  boxTransform = Transform{{0.0, 0.0, 0.0}, {{0.0, 0.0, -1.0}, 0.0}, {1.0, 1.0, 1.0}};
   auto  tri          = eng.createEntity(boxTransform, CachedTransform::fromTransform(boxTransform));
   auto& drawable     = tri.addComponent(Drawable3D{});

   drawable.addVertBuffer(std::vector{VertPosColor3D{{1.0, 1.0, 1.0}, 0xFF0000_rgbf},      // 0-FRU
                                      VertPosColor3D{{1.0, 1.0, -1.0}, 0xFF0000_rgbf},     // 1-BRU
                                      VertPosColor3D{{1.0, -1.0, 1.0}, 0xFF0000_rgbf},     // 2-FRD
                                      VertPosColor3D{{1.0, -1.0, -1.0}, 0xFF0000_rgbf},    // 3-BRD
                                      VertPosColor3D{{-1.0, 1.0, 1.0}, 0xFF0000_rgbf},     // 4-FLU
                                      VertPosColor3D{{-1.0, 1.0, -1.0}, 0xFF0000_rgbf},    // 5-BLU
                                      VertPosColor3D{{-1.0, -1.0, 1.0}, 0xFF0000_rgbf},    // 6-FLD
                                      VertPosColor3D{{-1.0, -1.0, -1.0}, 0xFF0000_rgbf}},  // 7-BLD
                          VertColor3D::Position{}, VertColor3D::Color{});

   drawable.setIndexBuffer(std::vector<Magnum::UnsignedInt>{
       2, 0, 1, 2, 1, 3,  // Right face
       6, 4, 5, 6, 5, 7,  // Left face
       1, 0, 4, 1, 4, 5,  // Top Face
       2, 3, 7, 2, 6, 7,  // Bottom Face
       3, 1, 5, 3, 5, 7,  // Back face
       6, 4, 0, 6, 0, 2   // Front Face
   });

   drawable.shader_ = std::make_shared<VertColor3D>();

   while (running) {
      eng.handleEvents().handleRender();
   }

   return 0;
}
