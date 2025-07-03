#pragma once

#include <memory>
#include <stdexcept>

#include <SDL3/SDL.h>

#define SDL_BIND_CREATE_AND_DESTROY(SDL_Type, SDL_CreateFunc, SDL_DestoryFunc) \
    template <> \
    struct Meta<SDL_Type> { \
        template <class ...Args> \
        static std::shared_ptr<SDL_Type> create(Args&&... args) { \
            auto ptr{::SDL_CreateFunc(std::forward<Args>(args)...)}; \
            if (nullptr == ptr) { \
                throw std::runtime_error{::SDL_GetError()}; \
            } \
            return std::shared_ptr<SDL_Type>(ptr, [](auto ptr) { \
                ::SDL_DestoryFunc(ptr); \
            }); \
        } \
    }

namespace SDL {

template <class SDL_Type>
struct Meta;

SDL_BIND_CREATE_AND_DESTROY(SDL_Renderer, SDL_CreateRenderer, SDL_DestroyRenderer);
SDL_BIND_CREATE_AND_DESTROY(SDL_Window, SDL_CreateWindow, SDL_DestroyWindow);

static_assert(std::is_same_v<::SDL_GLContext, SDL_GLContextState*>,
    "This datatype is available since SDL 3.2.0: typedef struct SDL_GLContextState *SDL_GLContext;");
SDL_BIND_CREATE_AND_DESTROY(SDL_GLContextState, SDL_GL_CreateContext, SDL_GL_DestroyContext);

template <>
struct Meta<SDL_GPUGraphicsPipeline> {
    static std::shared_ptr<SDL_GPUGraphicsPipeline> create(
        std::shared_ptr<SDL_GPUDevice> device,
        const SDL_GPUGraphicsPipelineCreateInfo *createinfo) {

        auto ptr{::SDL_CreateGPUGraphicsPipeline(device.get(), createinfo)};
        if (nullptr == ptr) {
            throw std::runtime_error{::SDL_GetError()};
        }
        return std::shared_ptr<SDL_GPUGraphicsPipeline>(ptr,
            [device=std::move(device)] (auto ptr) {
                ::SDL_ReleaseGPUGraphicsPipeline(device.get(), ptr);
            });
    }
};

}