module;
#include <SDL3/SDL.h>
#include <GL/glew.h>
module Window;

Window::Window(const std::string& title, int width, int height) {
    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create the SDL3 window
    window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Create the OpenGL context
    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        exit(1);
    }


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);       // Nearer fragments pass
    glDepthMask(GL_TRUE);       // Enable depth writing
    glClearDepth(1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Load the BMP icon
    SDL_Surface* iconSurface = SDL_LoadBMP("assets/icons/gameIcon.bmp");
    if (!iconSurface) {
        SDL_Log("Failed to load BMP icon: %s", SDL_GetError());
    }
    else {
        // Set the window icon
        SDL_SetWindowIcon(window, iconSurface);
        // Free the surface
        SDL_DestroySurface(iconSurface);
    }



    // Set the clear color (default background color)
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


    // Set the window as open
    isOpen = true;
}

Window::~Window() {
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers() {
    SDL_GL_SwapWindow(window);
}

bool Window::ShouldClose() const {
    return !isOpen;
}