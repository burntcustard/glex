#define GLEW_STATIC // Easier debugging
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>

#include <boost/program_options.hpp>

#define RUN_GRAPHICS_DISPLAY 0x00;

#include "common.h"
#include "GameWorld.h"
#include "Camera.h"

// Global variables (boo)
Camera camera(5.0, 0.0, 0.0); // Initialise the camera at xyz coords 5,0,0.


/*
 * SDL timers run in separate threads.  In the timer thread
 * push an event onto the event queue.  This event signifies
 * to call display() from the thread in which the OpenGL 
 * context was created.
 */
Uint32 tick(Uint32 interval, void *param) {
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.code = RUN_GRAPHICS_DISPLAY;
  event.user.data1 = 0;
  event.user.data2 = 0;
  SDL_PushEvent(&event);
  return interval;
}

struct SDLWindowDeleter {
  inline void operator()(SDL_Window* window) {
    SDL_DestroyWindow(window);
  }
};

void Update(const Uint8* keys, glm::vec2 &mouseDelta) {

  float mouseSensitivity = 0.01;

  float x = 0;
  float y = 0;
  float z = 0;

  if (mouseDelta.x || mouseDelta.y) {

    float pitch = 0; // Rotation around the x axis, i.e. looking up and down.
    float yaw   = 0; // Rotation around the y axis, i.e. looking left and right.
    float roll  = 0; // Rotation around the z axis, i.e. DO AN AILERON ROLL

    //std::cout << mouseDelta.x << "," << mouseDelta.y << std::endl;

    if (mouseDelta.x) {
      yaw = mouseSensitivity * mouseDelta.x;
    }
    if (mouseDelta.y) {
      pitch = mouseSensitivity * mouseDelta.y;
    }

    camera.Rotate(pitch, yaw, roll);

  }



  // Keyboard handling:

  if (keys[SDL_SCANCODE_W]) {
    z++;
    std::cout << "\rKey pressed: W" << std::flush;
  }
  if (keys[SDL_SCANCODE_A]) {
    x--;
    std::cout << "\rKey pressed: A" << std::flush;
  }
  if (keys[SDL_SCANCODE_S]) {
    z--;
  }
  if (keys[SDL_SCANCODE_D]) {
    x++;
  }
  if (keys[SDL_SCANCODE_SPACE]) {
    y++;
  }
  if (keys[SDL_SCANCODE_LSHIFT]) {
    y--;
  }
  if (keys[SDL_SCANCODE_ESCAPE]) {
    // This is NOT a good way to exit the game. It causes errors.
    // But it's here temporarily as a quick way to exit now that the mouse is being eaten.
    // It's bad partly because we try to quit here, then we still try to draw ~0.0001s afterwards.
	std::cout << "Escape pressed, trying to quit..." << std::endl;
    SDL_Quit();
  }

  camera.Move(x, y, z);

  mouseDelta = glm::vec2(0, 0);

}

void Draw(const std::shared_ptr<SDL_Window> window, const std::shared_ptr<GameWorld> game_world) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  game_world->Draw();

  // Don't forget to swap the buffers
  SDL_GL_SwapWindow(window.get());
}

std::shared_ptr<SDL_Window> InitWorld() {
  Uint32 width = 640;
  Uint32 height = 480;
  SDL_Window * _window;
  std::shared_ptr<SDL_Window> window;

  // Glew will later ensure that OpenGL 3 *is* supported
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Do double buffering in GL
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Initialise SDL - when using C/C++ it's common to have to
  // initialise libraries by calling a function within them.
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)<0) {
    std::cout << "Failed to initialise SDL: " << SDL_GetError() << std::endl;
    return nullptr;
  }

  // When we close a window quit the SDL application
  atexit(SDL_Quit);

  // Create a new window with an OpenGL surface
  _window = SDL_CreateWindow("Shader Example x"
                             , SDL_WINDOWPOS_CENTERED
                             , SDL_WINDOWPOS_CENTERED
                             , width
                             , height
                             , SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_GRABBED);
  if (!_window) {
    std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
    return nullptr;
  }

  SDL_ShowCursor(0); // Hide mouse cursor
  SDL_SetRelativeMouseMode(SDL_TRUE); // Capture mouse inside the window

  SDL_GLContext glContext = SDL_GL_CreateContext(_window);
  if (!glContext) {
    std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
    return nullptr;
  }

  // Initialise GLEW - an easy way to ensure OpenGl 3.0+
  // This *must* be done after we have set the video mode - otherwise we have no
  // OpenGL context to test.
  glewInit();
  if (!glewIsSupported("GL_VERSION_3_0")) {
    std::cerr << "OpenGL 3.0 not available" << std::endl;
    return nullptr;
  }

  // OpenGL settings
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  window.reset(_window, SDL_DestroyWindow);
  return window;
}

ApplicationMode ParseOptions (int argc, char ** argv) {
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()
     ("help", "print this help message")
     ("translate", "Show translation example (default)")
     ("rotate", "Show rotation example")
     ("scale", "Show scale example")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }

  if(vm.count("rotate")) {
    return ROTATE;
  }

  if(vm.count("scale")) {
    return SCALE;
  }

  // The default
  return TRANSFORM;
}

int main(int argc, char ** argv) {
  Uint32 delay = 1000/60; // in milliseconds

  // Pointer to a list of keys that are pressed, that automagically updates
  // every time SDL_PollEvent is (indirectly) called in the main event loop.
  const Uint8* keys = SDL_GetKeyboardState(NULL);

  bool quit = false;

  glm::vec2 mouseDelta = glm::vec2(0,0);

  auto mode = ParseOptions(argc, argv);
  auto window = InitWorld();
  auto game_world = std::make_shared<GameWorld>(mode);

  if(!window) {
    SDL_Quit();
  }

  // Call the function "tick" every delay milliseconds
  SDL_AddTimer(delay, tick, NULL);

  // Add the main event loop
  SDL_Event event;

  //while (!quit) {

    while (SDL_WaitEvent(&event)) {

      //std::cout << "beep";

      switch (event.type) {
        case SDL_QUIT:
          SDL_Quit();
          quit = true;
          std::cout << "SDL_Quit... ing" << std::endl;
          break;

        case SDL_MOUSEMOTION:
          mouseDelta.x = event.motion.xrel;
          mouseDelta.y = event.motion.yrel;
          break;

        case SDL_USEREVENT:
          Update(keys, mouseDelta);
          Draw(window, game_world);

          break;

        default:
          break;
      }
    }
  //}

}
