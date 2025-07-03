#include "App.hpp"

#include <stdexcept>

#include "SDL/SDL.hpp"

constexpr auto WINDOW_WIDTH{1080};
constexpr auto WINDOW_HEIGHT{720};

std::shared_ptr<App> g_app;

struct Vertex
{
    float x, y, z;      //vec3 position
    float r, g, b, a;   //vec4 color
};

static Vertex g_vertices[]
{
    { 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},   // top vertex
    {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},   // bottom left vertex
    { 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}    // bottom right vertex
};

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
    window_ = SDL::Meta<SDL_Window>::create(
        "hello gpu render",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);

    gpu_device_ = SDL::Meta<SDL_GPUDevice>::create(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL,
        false,
        nullptr);

    if (!SDL_ClaimWindowForGPUDevice(gpu_device_.get(), window_.get()) ) {
        throw std::runtime_error{::SDL_GetError()};
    }

    SDL_GPUBufferCreateInfo buffer_createinfo{};
    buffer_createinfo.size  = sizeof(g_vertices);
    buffer_createinfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    gpu_vertex_buffer_ = SDL::Meta<SDL_GPUBuffer>::create(gpu_device_, &buffer_createinfo);

    SDL_GPUTransferBufferCreateInfo transfer_buffer_createinfo{};
    transfer_buffer_createinfo.size  = sizeof(g_vertices);
    transfer_buffer_createinfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    gpu_transfer_buffer_ = SDL::Meta<SDL_GPUTransferBuffer>::create(gpu_device_, &transfer_buffer_createinfo);

    auto *data{static_cast<Vertex *>(SDL_MapGPUTransferBuffer(
        gpu_device_.get(),
        gpu_transfer_buffer_.get(),
        false)) };
    SDL_memcpy(data, g_vertices, sizeof(g_vertices));
    SDL_UnmapGPUTransferBuffer(gpu_device_.get(), gpu_transfer_buffer_.get());
}

App::~App()
{
    SDL_ReleaseWindowFromGPUDevice(gpu_device_.get(), window_.get());
}

void App::render()
{
    std::unique_ptr<SDL_GPUCommandBuffer, decltype(&SDL_SubmitGPUCommandBuffer)> command_buffer{
        SDL_AcquireGPUCommandBuffer(gpu_device_.get()), SDL_SubmitGPUCommandBuffer};

    {
        std::unique_ptr<SDL_GPUCopyPass, decltype(&SDL_EndGPUCopyPass)> gpu_copy_pass{
            SDL_BeginGPUCopyPass(command_buffer.get()), SDL_EndGPUCopyPass};

        SDL_GPUTransferBufferLocation location{};
        location.transfer_buffer = gpu_transfer_buffer_.get();
        location.offset = 0;

        SDL_GPUBufferRegion region{};
        region.buffer = gpu_vertex_buffer_.get();
        region.size = sizeof(g_vertices);
        region.offset = 0;

        SDL_UploadToGPUBuffer(
            gpu_copy_pass.get(),
            &location,
            &region,
            true);
    }

    {
        SDL_GPUTexture *swapchain_texture{};
        Uint32 width{}, height{};
        SDL_WaitAndAcquireGPUSwapchainTexture(
            command_buffer.get(),
            window_.get(),
            &swapchain_texture,
            &width,
            &height);

        if (nullptr == swapchain_texture) {
            return;
        }

        SDL_GPUColorTargetInfo color_target_info{};
        color_target_info.clear_color = SDL_FColor {
            .r=240/255.0f,
            .g=240/255.0f,
            .b=240/255.0f,
            .a=255/255.0f};
        color_target_info.load_op  = SDL_GPU_LOADOP_CLEAR;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;
        color_target_info.texture  = swapchain_texture;

        std::unique_ptr<SDL_GPURenderPass, decltype(&SDL_EndGPURenderPass)> gpu_render_pass{
            SDL_BeginGPURenderPass(
                command_buffer.get(),
                &color_target_info,
                1,
                nullptr),
            SDL_EndGPURenderPass};
    }
}
