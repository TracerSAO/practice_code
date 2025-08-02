#include "app.hpp"

#include <array>
#include <stdexcept>
#include <string>
#include <ranges>

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "i_homework.hpp"
#include "opengl/gl.hpp"

namespace {

constexpr auto WINDOW_WIDTH  {static_cast<int>(1080*0.8f)};
constexpr auto WINDOW_HEIGHT {static_cast<int>(1080*0.8f)};

std::shared_ptr<SDL_Window> window_;
std::shared_ptr<SDL::SDL_GLContext> gl_context_;

} // namespace

struct Demo : public IHomework
{
    GLuint VAO_{};
    GLuint VBO_{};
    GLuint EBO_{};
    std::shared_ptr<GL::ShaderProgram> gl_shader_program_;

    GLuint backgroud_texture_{};
    GLuint preview_texture_{};

    ~Demo() override {
        gl_shader_program_.reset();
        glDeleteBuffers(1, &VBO_);
        glDeleteBuffers(1, &EBO_);
        glDeleteTextures(1, &preview_texture_);
        glDeleteTextures(1, &backgroud_texture_);
        glDeleteVertexArrays(1, &VAO_);
    }

    void init() override {
        const GL::Shader vertex_shader{GL_VERTEX_SHADER, "shader/vertex.glsl"};
        const GL::Shader fragment_shader{GL_FRAGMENT_SHADER, "shader/fragment.glsl"};
        gl_shader_program_ = std::make_shared<GL::ShaderProgram>(vertex_shader, fragment_shader);

        {
            glGenTextures(1, &backgroud_texture_);
            GL::glCheckError();
            glBindTexture(GL_TEXTURE_2D, backgroud_texture_);
            GL::glCheckError();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            int32_t width{};
            int32_t height{};
            int32_t n_channels{};
            stbi_set_flip_vertically_on_load(true);
            uint8_t *image_data{stbi_load("./backgroud.png", &width, &height, &n_channels, 0)};
            if (nullptr == image_data) {
                throw std::runtime_error{"load texture failed"};
            }

            GLenum format{GL_RGB}; // 根据实际通道数设置格式
            if (n_channels == 4) { format = GL_RGBA; }
            else if (n_channels == 1) { format = GL_RED; }
            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        {
            glGenTextures(1, &preview_texture_);
            GL::glCheckError();
            glBindTexture(GL_TEXTURE_2D, preview_texture_);
            GL::glCheckError();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            int32_t width{};
            int32_t height{};
            int32_t n_channels{};
            stbi_set_flip_vertically_on_load(true);
            uint8_t *image_data{stbi_load("./preview.jpg", &width, &height, &n_channels, 0)};
            if (nullptr == image_data) {
                throw std::runtime_error{"load texture failed"};
            }

            GLenum format{GL_RGB}; // 根据实际通道数设置格式
            if (n_channels == 4) { format = GL_RGBA; }
            else if (n_channels == 1) { format = GL_RED; }
            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        gl_shader_program_->use();
        // glUniform1i(gl_shader_program_->getUniformLocation("texture1"), );


        constexpr std::array vertex{
            // x      y     z      r     g     b      u     v
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // left top
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // left botoom
             0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // right top
             0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f, // right botoom
        };
        constexpr std::array indices{
            0, 1, 2,  // first Triangle
            1, 2, 3   // second Triangle
        };

        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        glGenBuffers(1, &EBO_);
        {
            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), reinterpret_cast<void *>(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), reinterpret_cast<void *>(3*sizeof(float)) );
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), reinterpret_cast<void *>(6*sizeof(float)) );
            glEnableVertexAttribArray(2);

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

        glBindTexture(GL_TEXTURE_2D, backgroud_texture_);
        glBindVertexArray(VAO_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};

std::unique_ptr<IHomework> g_work;

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
        "Learn OpenGL",
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