#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include "opengl.hpp"
#include "SDL/SDL.hpp"

class App
{
public:
    App();
    ~App();
    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;

    static void Create();
    static void Destory();
    static void Render();

private:
    void initGpuPiepline();
    void initGpuVertexBuffer();

    void render();
    void renderImpl();

private:
    std::shared_ptr<SDL_Window> window_;
    std::shared_ptr<SDL::SDL_GLContext> gl_context_;
    std::shared_ptr<GL::GL_ShaderProgram> gl_shader_program_;

    GLuint VAO{};
    GLuint VBO{};
    GLuint EVO{};
};