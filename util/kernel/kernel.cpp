/**
 * @file kernel.cpp
 * @author Eron Ristich (eron@ristich.com)
 * @brief Generic control for a single SDL window with OpenGL
 * @version 0.1
 * @date 2022-07-19
 */

#include "kernel.h"

/**
 * @brief Construct a new Kernel object
 * 
 * @param title Title of the window
 * @param rx X dimension of the window in pixels
 * @param ry Y dimension of the window in pixels
 */
Kernel::Kernel(string title, int rx, int ry) : title(title), rx(rx), ry(ry) {
    cout << title << endl;
}

/**
 * @brief Destroy the Kernel object
 */
Kernel::~Kernel() {
    cout << "Removing kernel " << title << endl;
    SDL_DestroyRenderer(renderer);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);

    renderer = NULL;
    window = NULL;
}

/**
 * @brief Gets title of the kernel
 */
string Kernel::getTitle() {
    return title;
}

/**
 * @brief Gets x dimension of kernel in pixels
 */
int Kernel::getRX() {
    return rx;
}

/**
 * @brief Gets x dimension of kernel in pixels
 */
int Kernel::getRY() {
    return ry;
}

/**
 * @brief Gets the window object of the kernel
 */
SDL_Window* Kernel::getWindow() {
    return window;
}

/**
 * @brief Computes render data based on a function that determines the objects in the scene, and passes render to the screen. A valid renderer handler must have already been called in order to avoid undefined behavior.
 */
void Kernel::render() {
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render objects
    rendererHandler();

    // flush and render to SDL window (swaps render buffers)
    glFlush();
    SDL_GL_SwapWindow(window);
}

/**
 * @brief Initializes SDL functions
 * 
 * @return bool representing the success of the operation
 */
bool Kernel::initSDL() {
    if (SDL_Init(SDL_INIT_NOPARACHUTE) && SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    
    //Specify OpenGL Version (4.3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Log("SDL Initialized");
    return true;
}

/**
 * @brief Initializes gl functions and VSync (assumes gl context has already been defined)
 * 
 * @return bool representing the success of the operation
 */
bool Kernel::initGL() {
    // Specify shading technique
    glShadeModel(GL_SMOOTH);

    // Interpolation quality
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Initial color definition
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Check GLEW initialization
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if(error != GLEW_OK) {
        SDL_Log("Could not initialize GLEW: %s\n", glewGetErrorString(error));
        return false;
    } else {
        SDL_Log("GLEW initialized successfully");
        glViewport(0, 0, (GLsizei)SDL_GetWindowSurface(window)->w, (GLsizei)SDL_GetWindowSurface(window)->h);
    }

    //Use VSync (limit to refresh rate of monitor)
    if(SDL_GL_SetSwapInterval(1) < 0) {
        SDL_Log("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
        // does not return false, VSync is not essential to program
    } else {
        SDL_Log("VSync initialized");
    }
    return true;
}

/**
 * @brief Initializes SDL_Image image loading
 * 
 * @return bool representing the success of the operation
 */
bool Kernel::initIMG() {
    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        SDL_Log("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    SDL_Log("SDL_image initialized");
    return true;
}

/**
 * @brief Registers an event handler to manage events within the SDL window
 * 
 * @param f Function that handles SDL events (SDL_PollEvent)
 * @return bool representing the success of the operation
 */
bool Kernel::registerEventHandler(void (*f)()) {
    eventHandler = f;
    return true;
}

/**
 * @brief Registers a renderer handler to manage render calls to the OpenGL context
 * 
 * @param f Function that handles rendering updates
 * @return bool representing the success of the operation
 */
bool Kernel::registerRendererHandler(void (*f)()) {
    rendererHandler = f;
    return true;
}

/**
 * @brief Registers an update handler to manage updates in the objects within the SDL window
 * 
 * @param f Function that handles scene objects
 * @return bool representing the success of the operation
 */
bool Kernel::registerUpdateHandler(void (*f)()) {
    updateHandler = f;
    return true;
}

/**
 * @brief Registers a pre loop function to be called immediately before the render loop
 * 
 * @param f Function in question
 * @return bool representing the success of the operation
 */
bool Kernel::registerPreLoopStep(void (*f)()) {
    preLoopStep = f;
    return true;
}

/**
 * @brief Creates a window object with a registered OpenGL context
 * 
 * @param title Title of the window
 * @param rx X dimension in pixels
 * @param ry Y dimension in pixels
 * @return SDL_Window* pointer to SDL window
 */
SDL_Window* Kernel::createWindow(string title, int rx, int ry) {
    SDL_Window* wind = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        rx,
        ry,
        SDL_WINDOW_OPENGL
    );

    if(wind == NULL) {
        SDL_Log("Could not create window: %s\n", SDL_GetError());
        return NULL;
    }
    
    SDL_Log("Window successfully generated");
    return wind;
}

/**
 * @brief Creates a renderer object linked to the given window
 * 
 * @param wind Pointer to window object
 * @return SDL_Renderer* pointer to SDL renderer
 */
SDL_Renderer* Kernel::createRenderer(SDL_Window* wind) {
    SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED);
    
    if(rend == NULL) {
        SDL_Log("Could not create renderer: %s\n", SDL_GetError());
        return NULL;
    }
    
    SDL_Log("Renderer successfully generated");
    return rend;
}

/**
 * @brief Initializes necessary objects and starts the render loop
 */
void Kernel::start() {
    // init SDL
    if (!initSDL())
        throw std::runtime_error("SDL failed to initialize. Initialization failed");

    // create window
    window = createWindow(title, rx, ry);
    if (window == NULL)
        throw std::runtime_error("Window failed to be created. Initialization failed");

    // link renderer
    renderer = createRenderer(window);
    if (renderer == NULL)
        throw std::runtime_error("Renderer failed to be created. Initialization failed");

    // create gl context
    glContext = SDL_GL_CreateContext(window);

    // init openGL
    if (!initGL())
        throw std::runtime_error("OpenGL failed to initialize. Initialization failed");
    
    // init SDL Image
    if (!initIMG())
        throw std::runtime_error("SDL Image failed to initialize. Initialization failed");
    
    SDL_Log("Render loop started");
    running = true;
    
    preLoopStep();
    while (running) {
        // handle events
        eventHandler();

        // update objects
        updateHandler();

        // render objects
        render();
    }
    SDL_Log("Render loop stopped");
}

/**
 * @brief Stops the render loop. Must be called from within the render loop (e.g. by the event handler). Is not natively called!
 */
void Kernel::stop() {
    running = false;
}