#pragma once

#include <array>
#include <iostream>
#include <format>
#include <new>
#include <stdexcept>

#include <glad/glad.h>

namespace GL {

inline GLenum glCheckError_(const char *file, const char *function, int line)
{
    GLenum errorCode{GL_NO_ERROR};
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << function << ':' << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __PRETTY_FUNCTION__, __LINE__)

inline void APIENTRY glDebugOutput(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char *message,
    const void *userParam)
{
    (void)length;
    (void)userParam;

    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

class GL_Shader
{
public:
    GL_Shader(GLenum shader_type, const char *shader_source) {
        shader_id_ = glCreateShader(shader_type);
        glShaderSource(shader_id_, 1, &shader_source, nullptr);
        glCompileShader(shader_id_);

        GLint success{};
        glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &success);
        if (!success) {
            std::array<uint8_t, 512> log_info{};
            glGetShaderInfoLog(
                shader_id_,
                log_info.size(),
                nullptr,
                std::launder(reinterpret_cast<GLchar *>(log_info.data()) ));
            throw std::runtime_error{std::format("{}",
                std::string_view(reinterpret_cast<const char*>(log_info.data()), log_info.size()) )};
        }
    }
    ~GL_Shader() {
        glDeleteShader(shader_id_);
    }

    GLuint handle() const { return shader_id_; }

private:
    GLuint shader_id_{};
};

class GL_ShaderProgram
{
public:
    template <class ...Args>
    GL_ShaderProgram(Args&&... args) : shader_program_id_(glCreateProgram()) {
        (glAttachShader(shader_program_id_, std::forward<Args>(args)), ...);
        glLinkProgram(shader_program_id_);

        GLint success{};
        glGetProgramiv(shader_program_id_, GL_LINK_STATUS, &success);
        if (!success) {
            std::array<uint8_t, 512> log_info{};
            glGetProgramInfoLog(
                shader_program_id_,
                log_info.size(),
                nullptr,
                std::launder(reinterpret_cast<GLchar *>(log_info.data()) ));
            throw std::runtime_error{std::format("ERROR::SHADER::PROGRAM::LINKING_FAILED: {}",
                std::string_view(reinterpret_cast<const char*>(log_info.data()), log_info.size()) )};
        }
    }
    ~GL_ShaderProgram() {
        glDeleteProgram(shader_program_id_);
    }

    GLuint handle() const { return shader_program_id_; }

private:
    GLuint shader_program_id_{};
};

} // namespace GL