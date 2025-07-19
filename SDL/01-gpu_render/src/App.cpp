#include "app.hpp"

#include <array>
#include <format>
#include <stdexcept>

#include "SDL/SDL.hpp"

namespace {

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
    {.x= 0.0f, .y= 0.5f, .z=0.0f, .r=1.0f, .g=0.0f, .b=0.0f, .a=1.0f},   // top vertex
    {.x=-0.5f, .y=-0.5f, .z=0.0f, .r=1.0f, .g=1.0f, .b=0.0f, .a=1.0f},   // bottom left vertex
    {.x= 0.5f, .y=-0.5f, .z=0.0f, .r=1.0f, .g=0.0f, .b=1.0f, .a=1.0f}    // bottom right vertex
};

} // namespace

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
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error{std::format("SDL_Init failed, error={}", SDL_GetError()) };
    }

    window_ = SDL::Meta<SDL_Window>::create(
        "hello gpu render",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);

    gpu_device_ = SDL::Meta<SDL_GPUDevice>::create(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL,
        true,
        nullptr);

    if (!SDL_ClaimWindowForGPUDevice(gpu_device_.get(), window_.get()) ) {
        throw std::runtime_error{::SDL_GetError()};
    }

    initGpuPiepline();
    initGpuVertexBuffer();
}

App::~App()
{
    SDL_ReleaseWindowFromGPUDevice(gpu_device_.get(), window_.get());

    SDL_Quit();
}

void App::initGpuPiepline()
{
    size_t vertex_code_size{};
    size_t fragment_code_size{};
    std::unique_ptr<void, decltype(&SDL_free)> vertex_code {
        SDL_LoadFile("..\\..\\SDL\\01-gpu_render\\shader\\vertex.spv", &vertex_code_size),
        SDL_free};
    if (nullptr == vertex_code) {
        throw std::runtime_error{::SDL_GetError()};
    }
    std::unique_ptr<void, decltype(&SDL_free)> fragment_code {
        SDL_LoadFile("..\\..\\SDL\\01-gpu_render\\shader\\fragment.spv", &fragment_code_size),
        SDL_free};
    if (nullptr == fragment_code) {
        throw std::runtime_error{::SDL_GetError()};
    }

    SDL_GPUShaderCreateInfo vertex_shader_createinfo{};
    vertex_shader_createinfo.code                 = static_cast<Uint8 *>(vertex_code.get());
    vertex_shader_createinfo.code_size            = vertex_code_size;
    vertex_shader_createinfo.entrypoint           = "main";
    vertex_shader_createinfo.format               = SDL_GPU_SHADERFORMAT_SPIRV;
    vertex_shader_createinfo.stage                = SDL_GPU_SHADERSTAGE_VERTEX;
    vertex_shader_createinfo.num_samplers         = 0;
    vertex_shader_createinfo.num_storage_buffers  = 0;
    vertex_shader_createinfo.num_storage_textures = 0;
    vertex_shader_createinfo.num_uniform_buffers  = 0;
    auto vertex_shader{SDL::Meta<SDL_GPUShader>::create(gpu_device_, &vertex_shader_createinfo)};

    SDL_GPUShaderCreateInfo fragment_shader_createinfo{};
    fragment_shader_createinfo.code                 = static_cast<Uint8 *>(fragment_code.get());
    fragment_shader_createinfo.code_size            = fragment_code_size;
    fragment_shader_createinfo.entrypoint           = "main";
    fragment_shader_createinfo.format               = SDL_GPU_SHADERFORMAT_SPIRV;
    fragment_shader_createinfo.stage                = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fragment_shader_createinfo.num_samplers         = 0;
    fragment_shader_createinfo.num_storage_buffers  = 0;
    fragment_shader_createinfo.num_storage_textures = 0;
    fragment_shader_createinfo.num_uniform_buffers  = 0;
    auto fragment_shader{SDL::Meta<SDL_GPUShader>::create(gpu_device_, &fragment_shader_createinfo)};

    SDL_GPUGraphicsPipelineCreateInfo pipeline_createinfo{};
    pipeline_createinfo.vertex_shader   = vertex_shader.get();
    pipeline_createinfo.fragment_shader = fragment_shader.get();
    pipeline_createinfo.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    // describe the vertex buffers
    {
        std::array<SDL_GPUVertexBufferDescription, 1> vertex_buffer_descriptions{};
        vertex_buffer_descriptions[0].slot               = 0;
        vertex_buffer_descriptions[0].input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        vertex_buffer_descriptions[0].instance_step_rate = 0;
        vertex_buffer_descriptions[0].pitch              = sizeof(Vertex);

        pipeline_createinfo.vertex_input_state.num_vertex_buffers = vertex_buffer_descriptions.size();
        pipeline_createinfo.vertex_input_state.vertex_buffer_descriptions = vertex_buffer_descriptions.data();
    }

    // describe the vertex attribute
    {
        std::array<SDL_GPUVertexAttribute, 2> vertex_attributes{};
        // a_position
        vertex_attributes[0].buffer_slot = 0; // fetch data from the buffer at slot 0
        vertex_attributes[0].location    = 0; // layout (location = 0) in shader
        vertex_attributes[0].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; //vec3
        vertex_attributes[0].offset      = 0; // start from the first byte from current buffer position
        // a_color
        vertex_attributes[1].buffer_slot = 0; // use buffer at slot 0
        vertex_attributes[1].location    = 1; // layout (location = 1) in shader
        vertex_attributes[1].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; //vec4
        vertex_attributes[1].offset      = sizeof(float) * 3; // 4th float from current buffer position

        pipeline_createinfo.vertex_input_state.num_vertex_attributes = vertex_attributes.size();
        pipeline_createinfo.vertex_input_state.vertex_attributes = vertex_attributes.data();
    }

    // describe the color target
    {
        std::array<SDL_GPUColorTargetDescription, 1> color_target_descriptions{};
        color_target_descriptions[0] = {};
        color_target_descriptions[0].blend_state.enable_blend          = true;
        color_target_descriptions[0].blend_state.color_blend_op        = SDL_GPU_BLENDOP_ADD;
        color_target_descriptions[0].blend_state.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
        color_target_descriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
        color_target_descriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        color_target_descriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
        color_target_descriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        color_target_descriptions[0].format                            = SDL_GetGPUSwapchainTextureFormat(gpu_device_.get(), window_.get());

        pipeline_createinfo.target_info.num_color_targets = color_target_descriptions.size();
        pipeline_createinfo.target_info.color_target_descriptions = color_target_descriptions.data();
    }

    gpu_pipeline_ = SDL::Meta<SDL_GPUGraphicsPipeline>::create(gpu_device_, &pipeline_createinfo);
}

void App::initGpuVertexBuffer()
{
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

    {
        std::unique_ptr<SDL_GPUCommandBuffer, decltype(&SDL_SubmitGPUCommandBuffer)> command_buffer{
            SDL_AcquireGPUCommandBuffer(gpu_device_.get()), SDL_SubmitGPUCommandBuffer};

        std::unique_ptr<SDL_GPUCopyPass, decltype(&SDL_EndGPUCopyPass)> gpu_copy_pass{
            SDL_BeginGPUCopyPass(command_buffer.get()), SDL_EndGPUCopyPass};

        SDL_GPUTransferBufferLocation location{};
        location.transfer_buffer = gpu_transfer_buffer_.get();
        location.offset          = 0;

        SDL_GPUBufferRegion region{};
        region.buffer = gpu_vertex_buffer_.get();
        region.size   = sizeof(g_vertices);
        region.offset = 0;

        SDL_UploadToGPUBuffer(
            gpu_copy_pass.get(),
            &location,
            &region,
            true);
    }
}

void App::render()
{
    std::unique_ptr<SDL_GPUCommandBuffer, decltype(&SDL_SubmitGPUCommandBuffer)> command_buffer{
        SDL_AcquireGPUCommandBuffer(gpu_device_.get()), SDL_SubmitGPUCommandBuffer};

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

    std::array<SDL_GPUColorTargetInfo, 1> color_target_infos{};
    color_target_infos[0].clear_color = { .r=240/255.0f, .g=240/255.0f, .b=240/255.0f, .a=255/255.0f};
    color_target_infos[0].load_op     = SDL_GPU_LOADOP_CLEAR;
    color_target_infos[0].store_op    = SDL_GPU_STOREOP_STORE;
    color_target_infos[0].texture     = swapchain_texture;

    std::unique_ptr<SDL_GPURenderPass, decltype(&SDL_EndGPURenderPass)> gpu_render_pass{
        SDL_BeginGPURenderPass(
            command_buffer.get(),
            color_target_infos.data(),
            color_target_infos.size(),
            nullptr),
        SDL_EndGPURenderPass};
    SDL_BindGPUGraphicsPipeline(gpu_render_pass.get(), gpu_pipeline_.get());

    // bind the vertex buffer
    std::array<SDL_GPUBufferBinding, 1> bufferBindings{};
    bufferBindings[0].buffer = gpu_vertex_buffer_.get(); // index 0 is slot 0 in this example
    bufferBindings[0].offset = 0; // start from the first byte
    SDL_BindGPUVertexBuffers(
        gpu_render_pass.get(),
        0,
        bufferBindings.data(),
        bufferBindings.size()); // bind one buffer starting from slot 0

    // issue a draw call
    SDL_DrawGPUPrimitives(
        gpu_render_pass.get(),
        3,
        1,
        0,
        0);
}
