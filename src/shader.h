#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "debug.h"
#include "texture.h"


using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::mat4;


// -------- SHADERS --------
struct AttributeInfo
{
    GLint  size;
    GLenum type;
    GLint  index;
    std::unique_ptr<GLchar[]> name;
};

struct UniformInfo
{
    GLint  size;
    GLenum type;
    GLint  index;
    std::unique_ptr<GLchar[]> name;
};

struct UniformBufferInfo
{
    GLint  index;
    GLuint binding;
    std::unique_ptr<GLchar[]> name;
};


struct Shader
{
    GLuint id;
    std::unique_ptr<GLchar[]> name;
    std::unordered_map<std::string, AttributeInfo> attributes;
    std::unordered_map<std::string, UniformInfo>   uniforms;
    std::unordered_map<std::string, UniformInfo*>  samplers;
    std::unordered_map<std::string, UniformBufferInfo>  uniform_buffers;

    Shader() : id(0), name(nullptr), attributes(), uniforms(), samplers() {}

    Shader(
        GLuint program,
        std::unique_ptr<GLchar[]>&& name,
        std::unordered_map<std::string, AttributeInfo>&& attributes,
        std::unordered_map<std::string, UniformInfo>&&   uniforms,
        std::unordered_map<std::string, UniformInfo*>&&  samplers
    )
    {
        this->id         = program;
        this->name       = std::move(name);
        this->attributes = std::move(attributes);
        this->uniforms   = std::move(uniforms);
        this->samplers   = std::move(samplers);
    }

    Shader(Shader&& other) noexcept
    {
        this->id         = other.id;
        this->name       = std::move(name);
        this->attributes = std::move(attributes);
        this->uniforms   = std::move(uniforms);
        this->samplers   = std::move(samplers);
    }

    Shader& operator= (Shader&& other) noexcept
    {
        this->id         = other.id;
        this->name       = std::move(other.name);
        this->attributes = std::move(other.attributes);
        this->uniforms   = std::move(other.uniforms);
        this->samplers   = std::move(other.samplers);

        return *this;
    }
};

using ShaderMap = std::unordered_map<const char*, Shader>;


const char* TypeToString(GLenum type);

GLuint GetUniformLocation(const Shader& shader, const char* name);
void SetUniform(const Shader& shader, const char* name, bool value);
void SetUniform(const Shader& shader, const char* name, unsigned value);
void SetUniform(const Shader& shader, const char* name, float value);
void SetUniform(const Shader& shader, const char* name, const vec3& value);
void SetUniform(const Shader& shader, const char* name, const vec4& value);
void SetUniform(const Shader& shader, const char* name, const mat4& value);
void SetUniformArray(const Shader& shader, const char* name, const mat4* value, int count);
void SetTexture2D(const Shader& shader, const char* name, GLint index, const Texture& texture);
//void SetTexture3D(const Shader& shader, const char* name, GLint index, const Texture& texture);
void BindUniformBuffer(Shader& shader, const char* name, GLuint binding);

template <typename T> struct UBO { GLuint id; };

template <typename T>
UBO<T> CreateUniformBuffer(GLuint binding, const T* data = nullptr)
{
    ASSERT(sizeof(T) % 4 == 0, "Type must be padded to a multiple of 4");

    GLuint ubo;
    glGenBuffers(1, &ubo);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T), data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, binding, ubo, 0, sizeof(T));

    return { ubo };
}

template <typename T>
void SetUniformBuffer(UBO<T> ubo, const T& data, GLuint from_byte = 0, GLuint size = sizeof(T))
{
    ASSERT(sizeof(T) % 4 == 0, "Type must be padded to a multiple of 4");

    glBindBuffer(GL_UNIFORM_BUFFER, ubo.id);
    glBufferSubData(GL_UNIFORM_BUFFER, from_byte, size, &data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}



Shader CreateShader(const char* name, const char* vertex_source, const char* fragment_source, const char* geometry_source = nullptr);
void DeleteShader(Shader* shader);

struct InParam { const char *name, *vertex, *fragment, *geometry = nullptr; };
ShaderMap LoadShaders(const std::vector<InParam>& program_parameters);
struct InParamPost { const char *name, *fragment; };
ShaderMap LoadPostProcessingShaders(const char* base, const std::vector<InParamPost>& program_parameters);
