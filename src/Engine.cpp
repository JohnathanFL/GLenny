#include "Engine.hpp"

// std::vector<GL::DynamicAttribute> VertPosColor3D::attribs_ = {
//    GL::DynamicAttribute{GL::DynamicAttribute::Kind::GenericNormalized, 0, GL::DynamicAttribute::Components::Three,
//                         GL::DynamicAttribute::DataType::Float},
//    GL::DynamicAttribute{GL::DynamicAttribute::Kind::GenericNormalized, 1, GL::DynamicAttribute::Components::Four,
//                         GL::DynamicAttribute::DataType::Float}};

namespace std {
string to_string(const Magnum::Vector3& vec) {
   std::stringstream ss;
   ss << "Vec{" << vec.x() << ", " << vec.y() << ", " << vec.z() << "}";
   return ss.str();
}
}  // namespace std

VertColor3D::VertColor3D() {
   /* Load shader sources */
   GL::Shader vert{GL::Version::GL450, GL::Shader::Type::Vertex};
   GL::Shader frag{GL::Version::GL450, GL::Shader::Type::Fragment};
   vert.addSource(R"glsl(

                  layout(location = 0)
                  uniform mat4 transformationProjectionMatrix = mat4(1.0);

                  layout(location = 0)
                  in highp vec3 position;

                  layout(location = 1)
                  in lowp vec3 color;

                  out lowp vec4 interpolatedColor;

                  void main() {
                      gl_Position = transformationProjectionMatrix*vec4(position.xyz, 1.0);
                      interpolatedColor = vec4(color, 1.0);
                  })glsl");
   frag.addSource(R"glsl(

               in lowp vec4 interpolatedColor;

               out lowp vec4 fragmentColor;

               void main() {
                   fragmentColor = interpolatedColor;
               })glsl");

   /* Invoke parallel compilation for best performance */
   CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

   /* Attach the shaders */
   attachShaders({vert, frag});
   setLabel("VertColor3D");
   /* Link the program together */
   CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

Engine::Engine() : ctx_{Magnum::NoCreate} {
   using namespace Magnum;
   SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_SENSOR));

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
   window_ =
       SDL_CreateWindow("Glenny", 0, 0, width_, height_, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN);
   SDL_GL_MakeCurrent(window_, SDL_GL_CreateContext(window_));

   ctx_.create();


   primeFb_ = std::make_shared<GL::DefaultFramebuffer>();
}

Engine& Engine::handleEvents() {
   SDL_Event ev;
evLoop:
   while (SDL_PollEvent(&ev)) {
      for (auto handler : handlers_.sdl_)
         if (handler(ev))  // If the handler returns true, we consider the event to
                           // already be handled
            goto evLoop;   // May the great Bjorne forgive me

      switch (ev.type) {
         case SDL_QUIT:
            for (auto handler : handlers_.quit_)
               handler();
            break;

         case SDL_KEYDOWN:
         case SDL_KEYUP:
            for (auto handler : handlers_.key_)
               handler(ev.key.keysym, ev.type == SDL_KEYDOWN);
            break;

         case SDL_MOUSEMOTION:
            for (auto handler : handlers_.mouse_)
               handler(ev.motion);
            break;

         default:
            break;
      }
   }

   return *this;
}

Engine& Engine::handleRender() {
   using namespace Magnum::Math;
   using namespace Magnum::GL;
   GL::Renderer::setClearColor(0x2C3539FF_rgbaf);
   primeFb_->clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth | GL::FramebufferClear::Stencil);
   primeFb_->setViewport({{0, 0}, {1920, 1080}});

   for (auto camEnt : reg_.view<CachedTransform, Camera>()) {
      std::cerr << "Camera " << camEnt << std::endl;
      const auto&    cached = reg_.get<CachedTransform>(camEnt);
      const auto&    cam    = reg_.get<Camera>(camEnt);
      const auto&    view   = cached.cached_;
      Matrix4<float> proj   = cam.proj_ * cached.cached_;

      cam.fb_->bind();

      for (auto ent : reg_.view<CachedTransform, Transform, Drawable3D>()) {
         std::cerr << "\tDrawable " << ent << std::endl;
         const auto& transform = reg_.get<Transform>(ent);
         const auto& cached    = reg_.get<CachedTransform>(ent);
         const auto& drawable  = reg_.get<Drawable3D>(ent);

         const auto& model = cached.cached_;
         const auto& scale = transform.scale_;

         std::cerr << "\t\tdrawing " << drawable.mesh_->count() << " verts...\n";
         drawable.draw(model * Matrix4<float>::scaling(scale), proj);
      }
   }

   SDL_GL_SwapWindow(window_);
   return *this;
}
