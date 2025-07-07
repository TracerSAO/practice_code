#include "app.hpp"

#include <array>
#include <stdexcept>

namespace {

constexpr auto WINDOW_WIDTH  {static_cast<int>(1080*0.8f)};
constexpr auto WINDOW_HEIGHT {static_cast<int>(1080*0.8f)};

std::shared_ptr<SDL_Window> window_;
std::shared_ptr<SDL::SDL_GLContext> gl_context_;

void initSDLOpenGl()
{
    // 启用双缓冲：前台缓冲显示当前帧，后台缓冲绘制下一帧
    // 交换缓冲时可避免画面撕裂，提升渲染流畅性（默认开启）
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // 强制使用硬件加速渲染（GPU渲染）
    // 0=软件渲染(慢), 1=强制硬件加速, 不设置则自动选择
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // 配置颜色通道位数（RGBA各8位 = 32位色深/真彩色）
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);    // 红色通道256级色阶 (2^8=256)
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);    // 绿色通道256级色阶
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);    // 蓝色通道256级色阶
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);    // 透明度通道256级渐变

    // 设置深度缓冲区为24位（用于3D空间深度检测）
    // 存储每个像素的深度值(Z值)，决定物体前后遮挡关系
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // 设置模板缓冲区为8位（用于特殊渲染效果）
    // 可实现形状遮罩/轮廓描边/反射等特效
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // 设置主版本号为 4 (OpenGL 4.x)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    // 设置次版本号为 6 (OpenGL 4.6)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    // 指定使用核心配置文件（不含过时的固定管线函数）
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) ) {
        throw std::runtime_error{"gladLoadGLLoader load failed"};
	}
}

} // namespace

struct Demo {
    std::shared_ptr<GL::GL_ShaderProgram> gl_shader_program_;

    GLuint VAO{};
    GLuint VBO{};
    GLuint EVO{};

    ~Demo() {
        gl_shader_program_.reset();
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EVO);
        glDeleteVertexArrays(1, &VAO);
    }

    void init() {
        std::unique_ptr<void, decltype(&::SDL_free)> vertex_shader_source{
            SDL_LoadFile("../../SDL/02-opengl_render/01-base_opengl/shader/vertex.glsl", nullptr), SDL_free};
        if (nullptr == vertex_shader_source) {
            throw std::runtime_error{"vertex shader source open failed"};
        }
        std::unique_ptr<void, decltype(&::SDL_free)> fragment_shader_source{
            SDL_LoadFile("../../SDL/02-opengl_render/01-base_opengl/shader/fragment.glsl", nullptr), SDL_free};
        if (nullptr == fragment_shader_source) {
            throw std::runtime_error{"fragment shader source open failed"};
            return;
        }

        const GL::GL_Shader vertex_shader{GL_VERTEX_SHADER, static_cast<const char *>(vertex_shader_source.get())};
        const GL::GL_Shader fragment_shader{GL_FRAGMENT_SHADER, static_cast<const char *>(fragment_shader_source.get())};
        gl_shader_program_ = std::make_shared<GL::GL_ShaderProgram>(vertex_shader.handle(), fragment_shader.handle());

        constexpr std::array vertex{
            //   x      y     z
             0.5f,  0.5f, 0.0f,  // top right
             0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
        };
        constexpr std::array indices{
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EVO);
        {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EVO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, false, 3*sizeof(float), nullptr);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void render() {
        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制
        glUseProgram(gl_shader_program_->handle());
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
};

/**
 * @brief 使用 glDrawArrays 绘制相邻两个三角形
 */
struct Homework_1 {
    GLuint VAO{};
    GLuint VBO{};
    GLuint EVO{};
    std::shared_ptr<GL::GL_ShaderProgram> gl_shader_program_;

    ~Homework_1() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EVO);
    }

    void init() {
        // load gl shader
        {
            std::unique_ptr<void, decltype(&::SDL_free)> vertex_shader_source{
                SDL_LoadFile("../../SDL/02-opengl_render/01-base_opengl/shader/vertex.glsl", nullptr), SDL_free};
            if (nullptr == vertex_shader_source) {
                throw std::runtime_error{"vertex shader source open failed"};
            }
            std::unique_ptr<void, decltype(&::SDL_free)> fragment_shader_source{
                SDL_LoadFile("../../SDL/02-opengl_render/01-base_opengl/shader/fragment.glsl", nullptr), SDL_free};
            if (nullptr == fragment_shader_source) {
                throw std::runtime_error{"fragment shader source open failed"};
                return;
            }
            const GL::GL_Shader vertex_shader{GL_VERTEX_SHADER, static_cast<const char *>(vertex_shader_source.get())};
            const GL::GL_Shader fragment_shader{GL_FRAGMENT_SHADER, static_cast<const char *>(fragment_shader_source.get())};
            gl_shader_program_ = std::make_shared<GL::GL_ShaderProgram>(vertex_shader.handle(), fragment_shader.handle());
        }

        // init VAO/VBO/EVO
        {
            constexpr std::array Vertex{
                // x      y     z
                -0.5f, -0.5f, 0.0f,  // bottom left
                 0.0f, -0.5f, 0.0f,  // bottom middle
                 0.0f,  0.5f, 0.0f,  // top middle

                 0.0f, -0.5f, 0.0f,  // bottom middle
                 0.0f,  0.5f, 0.0f,  // top middle
                 0.5f,  0.5f, 0.0f,  // top right
            };
            constexpr std::array Indices{
                0, 1, 2,    // first
                1, 2, 3,    // second
            };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EVO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex), Vertex.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, EVO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Indices), Indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(VAO, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }

    void render() {
        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制
        glUseProgram(gl_shader_program_->handle());
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

struct Homework_2 {
    ~Homework_2() {

    }

    void init() {

    }

    void render() {

    }
};

// Demo g_demo;
std::unique_ptr<Demo> g_demo;

void App::Create()
{
    initSDLOpenGl();

    window_ = SDL::Meta<SDL_Window>::create(
        "hello gpu render",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);

    gl_context_ = SDL::Meta<SDL::SDL_GLContext>::create(window_.get());

    SDL_GL_MakeCurrent(window_.get(), gl_context_.get());
    SDL_GL_SetSwapInterval(1); // Enable vsync

    g_demo = std::make_unique<Demo>();
    g_demo->init();
}

void App::Destory()
{
    g_demo.reset();
}

void App::Render()
{
    // render
    g_demo->render();

    // 显示
    SDL_GL_SwapWindow(window_.get());
    SDL_Delay(1);
}