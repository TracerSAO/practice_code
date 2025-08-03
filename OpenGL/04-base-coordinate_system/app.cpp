#include "app.hpp"

#include <array>
#include <ranges>
#include <stdexcept>
#include <string>
#include <thread>

#include <glm/ext.hpp>
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

    GLuint backend_tex_{};
    GLuint frontend_tex_{};

    glm::mat4 model_mat_{1.0f};
    glm::mat4 view_mat_{1.0f};
    glm::mat4 projection_mat_{1.0f};

    ~Demo() override {
        gl_shader_program_.reset();
        glDeleteBuffers(1, &VBO_);
        glDeleteBuffers(1, &EBO_);
        glDeleteTextures(1, &frontend_tex_);
        glDeleteTextures(1, &backend_tex_);
        glDeleteVertexArrays(1, &VAO_);
    }

    void init() override {
        glEnable(GL_DEPTH_TEST);

        const GL::Shader vertex_shader{GL_VERTEX_SHADER, "shader/vertex.glsl"};
        const GL::Shader fragment_shader{GL_FRAGMENT_SHADER, "shader/fragment.glsl"};
        gl_shader_program_ = std::make_shared<GL::ShaderProgram>(vertex_shader, fragment_shader);

        {
            glGenTextures(1, &backend_tex_);
            GL::glCheckError();
            glBindTexture(GL_TEXTURE_2D, backend_tex_);
            GL::glCheckError();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            int32_t width{};
            int32_t height{};
            int32_t n_channels{};
            stbi_set_flip_vertically_on_load(true);
            uint8_t *image_data{stbi_load("./preview-backend.jpg", &width, &height, &n_channels, 0)};
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
            glGenTextures(1, &frontend_tex_);
            GL::glCheckError();
            glBindTexture(GL_TEXTURE_2D, frontend_tex_);
            GL::glCheckError();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            int32_t width{};
            int32_t height{};
            int32_t n_channels{};
            stbi_set_flip_vertically_on_load(true);
            uint8_t *image_data{stbi_load("./preview-frontend.jpg", &width, &height, &n_channels, 0)};
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
            gl_shader_program_->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, backend_tex_);
            glUniform1i(gl_shader_program_->getUniformLocation("u_backend_tex0"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, frontend_tex_);
            glUniform1i(gl_shader_program_->getUniformLocation("u_frontend_tex1"), 1);

            model_mat_ = glm::rotate(model_mat_, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            view_mat_ = glm::translate(view_mat_, glm::vec3(0.0f, 0.0f, -3.0f));
            projection_mat_ =
                glm::perspective(
                    glm::radians(45.0f),
                    static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
                    0.1f,
                    100.0f);
            glUniformMatrix4fv(gl_shader_program_->getUniformLocation("u_model_mat"), 1, GL_FALSE, glm::value_ptr(model_mat_));
            glUniformMatrix4fv(gl_shader_program_->getUniformLocation("u_view_mat"), 1, GL_FALSE, glm::value_ptr(view_mat_));
            glUniformMatrix4fv(gl_shader_program_->getUniformLocation("u_projection_mat"), 1, GL_FALSE, glm::value_ptr(projection_mat_));
        }

        constexpr std::array vertex{
            // x      y     z      r     g     b      u     v
            // -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // left top
            // -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // left botoom
            //  0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // right top
            //  0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f, // right botoom

            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };
        // constexpr std::array indices{
        //     0, 1, 2,  // first Triangle
        //     1, 2, 3   // second Triangle
        // };

        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        // glGenBuffers(1, &EBO_);
        {
            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex.data(), GL_STATIC_DRAW);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
            // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), reinterpret_cast<void *>(0));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), reinterpret_cast<void *>(3*sizeof(float)) );
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void render() override {
        constexpr glm::vec3 cubePositions[]{
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };
        static glm::mat4 model_arr[] {
            glm::translate(glm::mat4{1.0f}, cubePositions[0]),
            glm::translate(glm::mat4{1.0f}, cubePositions[1]),
            glm::translate(glm::mat4{1.0f}, cubePositions[2]),
            glm::translate(glm::mat4{1.0f}, cubePositions[3]),
            glm::translate(glm::mat4{1.0f}, cubePositions[4]),
            glm::translate(glm::mat4{1.0f}, cubePositions[5]),
            glm::translate(glm::mat4{1.0f}, cubePositions[6]),
            glm::translate(glm::mat4{1.0f}, cubePositions[7]),
            glm::translate(glm::mat4{1.0f}, cubePositions[8]),
            glm::translate(glm::mat4{1.0f}, cubePositions[9]),
        };

        // 清屏
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制
        gl_shader_program_->use();

        glBindVertexArray(VAO_);
        for (auto i{0}; i < 10; i++) {
            glm::mat4 &model = model_arr[i];
            if (i % 3 == 0) {
                float angle = 20.0f * static_cast<float>(i) + 10.0f;
                model = glm::rotate(model, glm::radians(angle) * 0.01f, glm::vec3(1.0f, 0.3f, 0.5f));
            }
            glUniformMatrix4fv(gl_shader_program_->getUniformLocation("u_model_mat"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glBindVertexArray(VAO_);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
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