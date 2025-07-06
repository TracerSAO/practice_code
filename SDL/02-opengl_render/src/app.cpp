#include "app.hpp"

#include <array>
#include <format>
#include <functional>
#include <numbers>
#include <ranges>
#include <stdexcept>
#include <string_view>

namespace {

constexpr auto WINDOW_WIDTH  {static_cast<int>(1080*1.2)};
constexpr auto WINDOW_HEIGHT {static_cast<int>(1080*1.2)};

std::shared_ptr<App> g_app;

} // namespace

// static void drawOpenCVLogo();

void App::Create()
{
    if (nullptr == g_app) {
        g_app = std::make_shared<App>();
    }
}

void App::Destory()
{
    if (nullptr != g_app) {
        g_app.reset();
    }
}

void App::Render()
{
    g_app->render();
}

App::App()
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

    window_ = SDL::Meta<SDL_Window>::create(
        "hello gpu render",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);

    gl_context_ = SDL::Meta<SDL::SDL_GLContext>::create(window_.get());

    SDL_GL_MakeCurrent(window_.get(), gl_context_.get());
    SDL_GL_SetSwapInterval(1); // Enable vsync

	if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) ) {
        throw std::runtime_error{"gladLoadGLLoader load failed"};
	}

    {
        std::unique_ptr<void, decltype(&::SDL_free)> vertex_shader_source{
            SDL_LoadFile("../../SDL/02-opengl_render/shader/vertex.glsl", nullptr), SDL_free};
        if (nullptr == vertex_shader_source) {
            throw std::runtime_error{"vertex shader source open failed"};
        }
        std::unique_ptr<void, decltype(&::SDL_free)> fragment_shader_source{
            SDL_LoadFile("../../SDL/02-opengl_render/shader/fragment.glsl", nullptr), SDL_free};
        if (nullptr == fragment_shader_source) {
            throw std::runtime_error{"fragment shader source open failed"};
            return;
        }

        const GL::GL_Shader vertex_shader{GL_VERTEX_SHADER, static_cast<const char *>(vertex_shader_source.get())};
        const GL::GL_Shader fragment_shader{GL_FRAGMENT_SHADER, static_cast<const char *>(fragment_shader_source.get())};
        gl_shader_program_ = std::make_shared<GL::GL_ShaderProgram>(vertex_shader.handle(), fragment_shader.handle());
    }

    {
        const std::array vertex{
             0.5f,  0.5f, 0.0f,  // top right
             0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
        };
        const std::array indices {
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
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

App::~App()
{
}

void App::render()
{
    // 清屏
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 绘制
    glUseProgram(gl_shader_program_->handle());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // 显示
    SDL_GL_SwapWindow(window_.get());
    SDL_Delay(1);
}

void App::renderImpl()
{

}

// static void drawOpenCVLogo()
// {
//     glEnable(GL_POINT_SMOOTH);
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glBegin(GL_TRIANGLES);
//     {
//         constexpr int   N{360};
//         constexpr float Pi{std::numbers::pi_v<float>};
//         constexpr float Radius{0.35f};
//         constexpr float Innder_Radius{0.35f/2};

//         constexpr auto draw_circle_but_kill_some_part = [N] (
//             SDL_FPoint const &offset,
//             SDL_FColor const &color,
//             std::function<bool(int)> const &func) {

//             constexpr auto To_Float{[](int v) { return static_cast<float>(v); }};

//             for (const auto i : std::views::iota(0, N)
//                     | std::views::filter(func)
//                     | std::views::transform(To_Float)) {

//                 const float angle{i / N * Pi * 2};
//                 const float next_angle{((i+1) / N) * Pi * 2};
//                 glColor3f(color.r, color.g, color.b);
//                 glVertex3f(offset.x + Radius * std::sinf(angle), offset.y + Radius * std::cosf(angle), 0.0f);
//                 glVertex3f(offset.x + Radius * std::sinf(next_angle), offset.y + Radius * std::cosf(next_angle), 0.0f);
//                 glVertex3f(offset.x + Innder_Radius * std::sinf(angle), offset.y + Innder_Radius * std::cosf(angle), 0.0f);

//                 glVertex3f(offset.x + Radius * std::sinf(next_angle), offset.y + Radius * std::cosf(next_angle), 0.0f);
//                 glVertex3f(offset.x + Innder_Radius * std::sinf(angle), offset.y + Innder_Radius * std::cosf(angle), 0.0f);
//                 glVertex3f(offset.x + Innder_Radius * std::sinf(next_angle), offset.y + Innder_Radius * std::cosf(next_angle), 0.0f);
//             }
//         };

//         constexpr float triangle_side_length{0.8f};
//         constexpr float angle_60{60 * (std::numbers::pi_v<float> / 180.0f)};
//         const SDL_FPoint a_point{.x= 0.0f,                   .y= float{std::sinf(angle_60)*triangle_side_length/2}};
//         const SDL_FPoint b_point{.x=-triangle_side_length/2, .y=-float{std::sinf(angle_60)*triangle_side_length/2}};
//         const SDL_FPoint c_point{.x= triangle_side_length/2, .y=-float{std::sinf(angle_60)*triangle_side_length/2}};
//         constexpr SDL_FColor a_color{.r= 1.0f, .g=0.0f, .b=0.0f, .a=0.0f};
//         constexpr SDL_FColor b_color{.r= 0.0f, .g=1.0f, .b=0.0f, .a=0.0f};
//         constexpr SDL_FColor c_color{.r= 0.0f, .g=0.0f, .b=1.0f, .a=0.0f};

//         // const SDL_FPoint x_point{.x= 0.0f, .y= 0.0f};
//         // constexpr SDL_FColor x_color{.r= 0.0f, .g=0.0f, .b=0.0f};
//         // draw_circle_but_kill_some_part(x_point, x_color, [] (int x) {
//         //     (void)x;
//         //     return true;
//         // });
//         draw_circle_but_kill_some_part(a_point, a_color, [N] (int x) {
//             constexpr auto rule_kill_left_circle = [] (int x) { return x/(N/12) != 5; };
//             constexpr auto rule_kill_right_th_circle = [] (int x) { return x/(N/12) != 6; };
//             return rule_kill_left_circle(x) && rule_kill_right_th_circle(x); (void)N;
//         });
//         draw_circle_but_kill_some_part(b_point, b_color, [N] (int x) {
//             constexpr auto rule_kill_left_circle = [] (int x) { return x/(N/12) != 1; };
//             constexpr auto rule_kill_right_th_circle = [] (int x) { return x/(N/12) != 2; };
//             return rule_kill_left_circle(x) && rule_kill_right_th_circle(x); (void)N;
//         });
//         draw_circle_but_kill_some_part(c_point, c_color, [N] (int x) {
//             constexpr auto rule_kill_left_circle = [] (int x) { return x/(N/12) != 0; };
//             constexpr auto rule_kill_right_th_circle = [] (int x) { return x/(N/12) != 11; };
//             return rule_kill_left_circle(x) && rule_kill_right_th_circle(x); (void)N;
//         });
//     }
//     glEnd();
// }