#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "debug.h"


#ifdef DEBUG
#define ASSERT_BOUND_SHADER(shader) do {GLint id; glGetIntegerv(GL_CURRENT_PROGRAM, &id); Assert(id == shader.id, "Shader '%s' is not bound.", shader.name.get()); } while(0)
#else
#define ASSERT_BOUND_SHADER(shader)
#endif


// -------- SHADERS --------
GLuint GetUniformLocation(const Shader& shader, const char* name)
{
    const auto& it = shader.uniforms.find(name);
    if (it == shader.uniforms.end())
    {
        Info("Uniform '%s' doesn't exist for shader '%s'", name, shader.name.get());
        return 0;
    }
    else
    {
        return it->second.index;
    }
}
void SetUniform(const Shader& shader, const char* name, bool value)
{
    ASSERT_BOUND_SHADER(shader);

    GLuint location = GetUniformLocation(shader, name);
    glUniform1i(location, value);
}
void SetUniform(const Shader& shader, const char* name, unsigned value)
{
    ASSERT_BOUND_SHADER(shader);

    GLuint location = GetUniformLocation(shader, name);
    glUniform1ui(location, value);
}
void SetUniform(const Shader& shader, const char* name, float value)
{
    ASSERT_BOUND_SHADER(shader);

    GLuint location = GetUniformLocation(shader, name);
    glUniform1f(location, value);
}
void SetUniform(const Shader& shader, const char* name, const vec3& value)
{
    ASSERT_BOUND_SHADER(shader);

    GLuint location = GetUniformLocation(shader, name);
    glUniform3f(location, value.x, value.y, value.z);
}
void SetUniform(const Shader& shader, const char* name, const vec4& value)
{
    ASSERT_BOUND_SHADER(shader);

    GLuint location = GetUniformLocation(shader, name);
    glUniform4f(location, value.x, value.y, value.z, value.w);
}
void SetUniform(const Shader& shader, const char* name, const mat4& value)
{
    ASSERT_BOUND_SHADER(shader);

    GLuint location = GetUniformLocation(shader, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void SetUniformArray(const Shader& shader, const char* name, const mat4* value, int count)
{
    ASSERT_BOUND_SHADER(shader);

    GLuint location = GetUniformLocation(shader, name);
    glUniformMatrix4fv(location, count, GL_FALSE, (const GLfloat *) value);
}
//void SetTexture2D(const Shader& shader, const char* name, GLint index, const Texture& texture)
//{
//    ASSERT_BOUND_SHADER(shader);
//    Assert(texture.dimension == 2, "Texture '%s' has wrong dimension (%d).", texture.name.c_str(), texture.dimension);
//
//    // NOTE(ted): We MUST bind a texture for all samplers before drawing.
////    Assert(shader.samplers.size() > index, "Index %i specify a greater number than the amount of samplers (%i) for program '%s'.", index, shader.samplers.size(), shader.name);
//
//    GLuint location = GetUniformLocation(shader, name);
//    glActiveTexture(GL_TEXTURE0 + index);
//    glBindTexture(GL_TEXTURE_2D, texture.id);
//    glUniform1i(location, index);
//}
//void SetTexture3D(const Shader& shader, const char* name, GLint index, const Texture& texture)
//{
//    ASSERT_BOUND_SHADER(shader);
//    Assert(texture.dimension == 3, "Texture '%s' has wrong dimension (%d).", texture.name.c_str(), texture.dimension);
//
//    // NOTE(ted): We MUST bind a texture for all samplers before drawing.
////    Assert(shader.samplers.size() > index, "Index %i specify a greater number than the amount of samplers (%i) for program '%s'.", index, shader.samplers.size(), shader.name);
//
//    GLuint location = GetUniformLocation(shader, name);
//    glActiveTexture(GL_TEXTURE0 + index);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
//    glUniform1i(location, index);
//}

void BindUniformBuffer(Shader& shader, const char* name, GLuint binding)
{
    GLint index = glGetUniformBlockIndex(shader.id, name);
    Assert(index != -1, "Invalid uniform block for shader %s!", shader.name.get());
    glUniformBlockBinding(shader.id, index, binding);


    int length = strnlen(name, 127);
    auto uniform_buffer_name = std::make_unique<GLchar[]>(length + 1);
    strncpy(uniform_buffer_name.get(), name, length);
    uniform_buffer_name.get()[length] = '\0';

    shader.uniform_buffers[name] = UniformBufferInfo{ .index = index, .binding = binding, .name = std::move(uniform_buffer_name) };
}

Shader CreateShader(const char* name, const char* vertex_source, const char* fragment_source, const char* geometry_source)
{
    Assert(*vertex_source != 0,   "Vertex source cannot be empty");
    Assert(*fragment_source != 0, "Fragment source cannot be empty");
    if (geometry_source)
        Assert(*geometry_source != 0, "Geometry source cannot be empty");

    constexpr int log_size = 512;

    GLint success;
    char  info_log[log_size];

    // Allocate, assign and compile the vertex shader.
    GLuint vertex;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, nullptr);
    glCompileShader(vertex);

    // Allocate, assign and compile the fragment shader.
    GLuint fragment;
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, nullptr);
    glCompileShader(fragment);

    GLuint geometry = 0;
    if (geometry_source)
    {
        // Allocate, assign and compile the geometry shader.
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometry_source, nullptr);
        glCompileShader(geometry);
    }

    // Allocate and attach the compiled shaders. Then link the attached shaders into a program.
    GLuint program;
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    if (geometry)
        glAttachShader(program, geometry);
    glLinkProgram(program);


    // ---- DEBUG ----
    // Check shaders compile status and programs link status (and validate status?).
    if (!program)
    {
        glGetShaderInfoLog(vertex, log_size, nullptr, info_log);
        Warning("Invalid shader program for program '%s'.\n%s", name, info_log);
    }
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, log_size, nullptr, info_log);
        Warning("Vertex shader compilation failed for program '%s'.\n%s", name, info_log);
    }
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, log_size, nullptr, info_log);
        Warning("Fragment shader compilation failed for program '%s'.\n%s", name, info_log);
    }
    if (geometry)
    {
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, log_size, nullptr, info_log);
            Warning("Fragment shader compilation failed for program '%s'.\n%s", name, info_log);
        }
    }
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, log_size, nullptr, info_log);
        Warning("Shader program linking failed for program '%s'.\n%s", name, info_log);
    }


    // ---- QUERY ATTRIBUTES ----
    std::unordered_map<std::string, AttributeInfo> attributes;
    {
        GLint max_name_length = 0;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_length);

        GLchar* buffer = new GLchar[max_name_length];

        GLint   count  = 0;
        GLint   size   = 0;   // Size of the variable.
        GLenum  type   = 0;   // Type of the variable (float, vec3 or mat4, etc).
        GLsizei length = 0;   // Numbers of characters written to the buffer.
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);

        attributes.reserve(count);
        for (int i = 0; i < count; i++)
        {
            glGetActiveAttrib(program, (GLuint)i, max_name_length, &length, &size, &type, buffer);
            GLint index = glGetAttribLocation(program, buffer);

            auto attribute_name = std::make_unique<GLchar[]>(length+1);
            strncpy(attribute_name.get(), buffer, length);
            attribute_name.get()[length] = '\0';

            attributes[buffer] = AttributeInfo{
                .name  = std::move(attribute_name),
                .size  = size,
                .type  = type,
                .index = index,
            };
        }

        delete[] buffer;
    }

    // ---- QUERY UNIFORMS ----
    std::unordered_map<std::string, UniformInfo>  uniforms;
    std::unordered_map<std::string, UniformInfo*> samplers;
    {
        GLint max_name_length = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);

        GLchar* buffer = new GLchar[max_name_length];

        GLint   count  = 0;
        GLint   size   = 0;   // Size of the variable.
        GLenum  type   = 0;   // Type of the variable (float, vec3 or mat4, etc).
        GLsizei length = 0;   // Numbers of characters written to the buffer.
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

        uniforms.reserve(count);
        for (int i = 0; i < count; i++)
        {
            glGetActiveUniform(program, (GLuint)i, max_name_length, &length, &size, &type, buffer);
            GLint index = glGetUniformLocation(program, buffer);

            auto uniform_name = std::make_unique<GLchar[]>(length+1);
            strncpy(uniform_name.get(), buffer, length);
            uniform_name.get()[length] = '\0';

            uniforms[buffer] = UniformInfo{
                    .name  = std::move(uniform_name),
                    .size  = size,
                    .type  = type,
                    .index = index,
            };

            if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)  // NOTE(ted): We only support two types for now.
                samplers[buffer] = &uniforms[buffer];
        }

        delete[] buffer;
    }

    // Delete the shaders as they are no longer needed when we have the program (unless we'd like to reuse one, or more,
    // of the shaders to create a new program).
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    auto name_length  = strnlen(name, 255);
    auto program_name = std::make_unique<GLchar[]>(name_length+1);
    strncpy(program_name.get(), name, name_length+1);

    Shader shader(program, std::move(program_name), std::move(attributes), std::move(uniforms), std::move(samplers));
    return shader;
}


void DeleteShader(Shader* shader)
{
    glDeleteProgram(shader->id);
//    for (size_t i = 0; i < loaded_shaders.size(); ++i)
//    {
//        if (&loaded_shaders[i] == shader)
//        {
//            loaded_shaders.erase(loaded_shaders.begin() + i);
//            break;
//        }
//    }
}

ShaderMap LoadShaders(const std::vector<InParam>& program_parameters)
{
    ShaderMap programs;  // TODO(ted): Pre-allocate.
    for (const auto& info : program_parameters)
    {
        const char* program_name = info.name;
        const std::unique_ptr<char> vertex_source   = LoadFileToString(info.vertex);
        const std::unique_ptr<char> fragment_source = LoadFileToString(info.fragment);
        const std::unique_ptr<char> geometry_source = info.geometry ? LoadFileToString(info.geometry) : nullptr;

        programs[program_name] = CreateShader(program_name, vertex_source.get(), fragment_source.get(), geometry_source.get());
    }

    return programs;
}

ShaderMap LoadPostProcessingShaders(const char* base, const std::vector<InParamPost>& program_parameters)
{
    ShaderMap programs;  // TODO(ted): Pre-allocate.
    {
        const std::unique_ptr<char> post_processing_vertex_source = LoadFileToString(base);
        for (const auto& info : program_parameters)
        {
            const char* program_name    = info.name;
            const std::unique_ptr<char> fragment_source = LoadFileToString(info.fragment);

            programs[program_name] = CreateShader(program_name, post_processing_vertex_source.get(), fragment_source.get());
        }
    }

    return programs;
}


const char* TypeToString(GLenum type)
{
    switch (type)
    {
        case GL_FLOAT: return "GL_FLOAT";
        case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
        case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
        case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
        case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
        case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
        case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
        case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
        case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
        case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
        case GL_INT: return "GL_INT";
        case GL_INT_VEC2: return "GL_INT_VEC2";
        case GL_INT_VEC3: return "GL_INT_VEC3";
        case GL_INT_VEC4: return "GL_INT_VEC4";
        case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
        case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
        case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
        case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
        case GL_DOUBLE: return "GL_DOUBLE";
        case GL_DOUBLE_VEC2: return "GL_DOUBLE_VEC2";
        case GL_DOUBLE_VEC3: return "GL_DOUBLE_VEC3";
        case GL_DOUBLE_VEC4: return "GL_DOUBLE_VEC4";
        case GL_DOUBLE_MAT2: return "GL_DOUBLE_MAT2";
        case GL_DOUBLE_MAT3: return "GL_DOUBLE_MAT3";
        case GL_DOUBLE_MAT4: return "GL_DOUBLE_MAT4";
        case GL_DOUBLE_MAT2x3: return "GL_DOUBLE_MAT2x3";
        case GL_DOUBLE_MAT2x4: return "GL_DOUBLE_MAT2x4";
        case GL_DOUBLE_MAT3x2: return "GL_DOUBLE_MAT3x2";
        case GL_DOUBLE_MAT3x4: return "GL_DOUBLE_MAT3x4";
        case GL_DOUBLE_MAT4x2: return "GL_DOUBLE_MAT4x2";
        case GL_DOUBLE_MAT4x3: return "GL_DOUBLE_MAT4x3";
        case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
        case GL_BOOL: return "GL_BOOL";
        default: return "UNKNOWN_TYPE";
    }
}