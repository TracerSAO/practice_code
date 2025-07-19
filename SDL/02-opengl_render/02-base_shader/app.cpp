#include "app.hpp"

#include <array>
#include <stdexcept>
#include <string>
#include <ranges>

#include "opengl/gl.hpp"

namespace {

constexpr auto WINDOW_WIDTH  {static_cast<int>(1080*0.8f)};
constexpr auto WINDOW_HEIGHT {static_cast<int>(1080*0.8f)};

std::shared_ptr<SDL_Window> window_;
std::shared_ptr<SDL::SDL_GLContext> gl_context_;

} // namespace

struct IFrameWork {
    virtual ~IFrameWork() = default;

    virtual void init() = 0;
    virtual void render() = 0;
};

struct Demo : public IFrameWork {
    GLuint VAO{};
    GLuint VBO{};
    // GLuint EBO{};
    std::shared_ptr<GL::ShaderProgram> gl_shader_program_;

    ~Demo() override {
        gl_shader_program_.reset();
        glDeleteBuffers(1, &VBO);
        // glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void init() override {
        const GL::Shader vertex_shader{GL_VERTEX_SHADER, "shader/vertex.glsl"};
        const GL::Shader fragment_shader{GL_FRAGMENT_SHADER, "shader/fragment.glsl"};
        gl_shader_program_ = std::make_shared<GL::ShaderProgram>(vertex_shader, fragment_shader);

        constexpr std::array vertex{
            // x      y     z   //   r     g     b
             0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f, // bottom left
             0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f, // top left
        };
        // constexpr std::array indices{
        //     0, 1, 3,  // first Triangle
        //     1, 2, 3   // second Triangle
        // };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex.data(), GL_STATIC_DRAW);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), reinterpret_cast<void *>(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), reinterpret_cast<void *>(3*sizeof(float)) );
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void render() override {
        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制
        gl_shader_program_->use();
        const auto x_pos_offset_location{gl_shader_program_->getUniformLocation("x_pos_offset")};
        glUniform1f(x_pos_offset_location, 0.5);

        glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};

std::unique_ptr<IFrameWork> g_work;

void App::Create()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error{"SDL init failed"};
    }

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
#if !defined(NDEBUG)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    }

    window_ = SDL::Meta<SDL_Window>::create(
        "hello opengl",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);

    gl_context_ = SDL::Meta<SDL::SDL_GLContext>::create(window_.get());

    SDL_GL_MakeCurrent(window_.get(), gl_context_.get());
    SDL_GL_SetSwapInterval(1); // Enable vsync
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) ) {
        throw std::runtime_error{"gladLoadGLLoader load failed"};
	}

    int flags{};
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) { // enable OpenGL debug context if context allows for debug context
        // SDL_assert(false);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        glDebugMessageCallback(GL::glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    g_work = std::make_unique<Demo>();
    g_work->init();
}

void App::Destory()
{
    g_work.reset();

    SDL_Quit();
}

void App::Render()
{
    // render
    g_work->render();

    // 显示
    SDL_GL_SwapWindow(window_.get());
    SDL_Delay(1);
}