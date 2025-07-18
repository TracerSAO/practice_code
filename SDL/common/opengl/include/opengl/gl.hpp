#pragma once

#include <array>
#include <format>
#include <new>
#include <stdexcept>

#include <glad/glad.h>

namespace GL {

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