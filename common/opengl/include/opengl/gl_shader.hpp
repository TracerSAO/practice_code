#pragma once

#include <array>
#include <cinttypes>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <glad/glad.h>

#include "gl_error.hpp"

namespace GL {

inline GLuint make_shader(GLenum shader_type, const std::filesystem::path &shader_code_path)
{
    std::ifstream shader_file;
    std::string shader_code_str;
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shader_file.open(shader_code_path);
        std::stringstream stream;
        stream << shader_file.rdbuf();
        shader_file.close();
        shader_code_str = stream.str();
    }
    catch (const std::ifstream::failure &error) {
        throw std::runtime_error{std::format(
            "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ code:{} what:{}",
            error.code().value(), error.what()) };
    }

    GLuint shader_id{glCreateShader(shader_type)};
    const char *tmp_ptr{shader_code_str.data()};
    glShaderSource(shader_id, 1, &tmp_ptr, nullptr);
    glCompileShader(shader_id);

    GLint success{};
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::array<uint8_t, g_DEFAULT_ERROR_LOG_SIZZE> log_info{};
        glGetShaderInfoLog(
            shader_id,
            log_info.size(),
            nullptr,
            std::launder(reinterpret_cast<GLchar *>(log_info.data()) ));
        throw std::runtime_error{std::format(
            "ERROR::SHADER::COMPILATION_FAILED shader_type:{} what:{}",
            shader_type,
            std::string_view(reinterpret_cast<const char*>(log_info.data()), log_info.size()) )};
    }

    return shader_id;
}

template <class ...Args>
inline GLuint make_shader_program(Args&&... args)
{
    auto shader_program_id{glCreateProgram()};

    (glAttachShader(shader_program_id, std::forward<Args>(args)), ...);
    glLinkProgram(shader_program_id);

    GLint success{};
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success) {
        std::array<uint8_t, g_DEFAULT_ERROR_LOG_SIZZE> log_info{};
        glGetProgramInfoLog(
            shader_program_id,
            log_info.size(),
            nullptr,
            std::launder(reinterpret_cast<GLchar *>(log_info.data()) ));
        throw std::runtime_error{
            std::format("ERROR::SHADER::PROGRAM::LINKING_FAILED what:{}",
            std::string_view(reinterpret_cast<const char*>(log_info.data()), log_info.size()) )};
    }

    return shader_program_id;
}

class Shader
{
public:
    Shader(GLenum shader_type, const std::filesystem::path &shader_code_path) {
        shader_id_ = make_shader(shader_type, shader_code_path);
    }
    ~Shader() {
        if (0 != shader_id_) {
            glDeleteShader(shader_id_);
        }
    }
    Shader(const Shader &) = delete;
    Shader(Shader &&) = delete;
    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

    GLuint handle() const {
        return shader_id_;
    }

private:
    GLuint shader_id_{};
};

class ShaderProgram
{
public:
    ShaderProgram(const Shader &vertex_shader, const Shader &fragment_shader) {
        shader_program_id_ = make_shader_program(vertex_shader.handle(), fragment_shader.handle());
    }
    ~ShaderProgram() {
        if (0 != shader_program_id_) {
            glDeleteProgram(shader_program_id_);
        }
    }
    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram(ShaderProgram &&) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;
    ShaderProgram &operator=(ShaderProgram &&) = delete;

    GLuint handle() const {
        return shader_program_id_;
    }

    void use() const {
        check_shader_program_id();
        glUseProgram(shader_program_id_);
    }

    GLint getUniformLocation(std::string_view uniform_name) const {
        check_shader_program_id();
        return glGetUniformLocation(shader_program_id_, uniform_name.data());
    }

private:
    void check_shader_program_id() const {
        if (0 == shader_program_id_) {
            throw std::runtime_error{"ERROR::SHADER::PROGRAM what:invalid program_id"};
        }
    }

private:
    GLuint shader_program_id_;
};

} // namespace GL