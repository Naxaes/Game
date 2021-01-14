#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <optional>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "debug.h"

using glm::vec2;
using glm::vec3;
using glm::mat4;

static constexpr vec3 RED     {1.0f, 0.0f, 0.0f};
static constexpr vec3 GREEN   {0.0f, 1.0f, 0.0f};
static constexpr vec3 BLUE    {0.0f, 0.0f, 1.0f};
static constexpr vec3 YELLOW  {1.0f, 1.0f, 0.0f};
static constexpr vec3 MAGENTA {1.0f, 0.0f, 1.0f};
static constexpr vec3 CYAN    {0.0f, 1.0f, 1.0f};
static constexpr vec3 WHITE   {1.0f, 1.0f, 1.0f};
static constexpr vec3 BLACK   {0.0f, 0.0f, 0.0f};

static constexpr vec3 WORLD_AXIS_RIGHT   = vec3(1.0f,  0.0f,  0.0f);
static constexpr vec3 WORLD_AXIS_UP      = vec3(0.0f,  1.0f,  0.0f);
static constexpr vec3 WORLD_AXIS_FORWARD = vec3(0.0f,  0.0f, -1.0f);

enum class RendrererAPI { None, OpenGL };
static const RendrererAPI RENDERER_API = RendrererAPI::OpenGL;

static bool context_created = false;


enum class ShaderDataType
{
    None = 0, Float1, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool,
    Texture2D
};


static ShaderDataType OpenGLTypeToShaderDataType(GLenum type)
{
    switch (type)
    {
        case GL_FLOAT:       return ShaderDataType::Float1;
        case GL_FLOAT_VEC2:  return ShaderDataType::Float2;
        case GL_FLOAT_VEC3:  return ShaderDataType::Float3;
        case GL_FLOAT_VEC4:  return ShaderDataType::Float4;
        case GL_FLOAT_MAT3:  return ShaderDataType::Mat3;
        case GL_FLOAT_MAT4:  return ShaderDataType::Mat4;
        case GL_INT:         return ShaderDataType::Int;
        case GL_INT_VEC2:    return ShaderDataType::Int2;
        case GL_INT_VEC3:    return ShaderDataType::Int3;
        case GL_INT_VEC4:    return ShaderDataType::Int4;
        case GL_BOOL:        return ShaderDataType::Bool;
        case GL_SAMPLER_2D:  return ShaderDataType::Texture2D;
        case 0:              return ShaderDataType::None;
        default: ERROR("Unknown data type!");
    }
}



static std::uint32_t ShaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float1:    return 4;
        case ShaderDataType::Float2:    return 4 * 2;
        case ShaderDataType::Float3:    return 4 * 3;
        case ShaderDataType::Float4:    return 4 * 4;
        case ShaderDataType::Mat3:      return 4 * 3 * 3;
        case ShaderDataType::Mat4:      return 4 * 4 * 4;
        case ShaderDataType::Int:       return 4;
        case ShaderDataType::Int2:      return 4 * 2;
        case ShaderDataType::Int3:      return 4 * 3;
        case ShaderDataType::Int4:      return 4 * 4;
        case ShaderDataType::Bool:      return 1;
        case ShaderDataType::Texture2D: return 0;
        case ShaderDataType::None:      return 0;
        default: ERROR("Unknown data type!");
    }
}

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float1:    return GL_FLOAT;
        case ShaderDataType::Float2:    return GL_FLOAT;
        case ShaderDataType::Float3:    return GL_FLOAT;
        case ShaderDataType::Float4:    return GL_FLOAT;
        case ShaderDataType::Mat3:      return GL_FLOAT;
        case ShaderDataType::Mat4:      return GL_FLOAT;
        case ShaderDataType::Int:       return GL_INT;
        case ShaderDataType::Int2:      return GL_INT;
        case ShaderDataType::Int3:      return GL_INT;
        case ShaderDataType::Int4:      return GL_INT;
        case ShaderDataType::Bool:      return GL_BOOL;
        case ShaderDataType::Texture2D: return GL_SAMPLER_2D;
        case ShaderDataType::None:      return 0;
        default: ERROR("Unknown data type!");
    }
}

static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float1:    return GL_FLOAT;
        case ShaderDataType::Float2:    return GL_FLOAT_VEC2;
        case ShaderDataType::Float3:    return GL_FLOAT_VEC3;
        case ShaderDataType::Float4:    return GL_FLOAT_VEC4;
        case ShaderDataType::Mat3:      return GL_FLOAT_MAT3;
        case ShaderDataType::Mat4:      return GL_FLOAT_MAT4;
        case ShaderDataType::Int:       return GL_INT;
        case ShaderDataType::Int2:      return GL_INT_VEC2;
        case ShaderDataType::Int3:      return GL_INT_VEC3;
        case ShaderDataType::Int4:      return GL_INT_VEC4;
        case ShaderDataType::Bool:      return GL_BOOL;
        case ShaderDataType::Texture2D: return GL_SAMPLER_2D;
        case ShaderDataType::None:      return 0;
        default: ERROR("Unknown data type!");
    }
}

//https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
std::uint32_t GetComponentCount(const ShaderDataType& type)
{
    switch (type)
    {
        case ShaderDataType::Float1:   return 1;
        case ShaderDataType::Float2:    return 2;
        case ShaderDataType::Float3:    return 3;
        case ShaderDataType::Float4:    return 4;
        case ShaderDataType::Mat3:      return 3; // 3* float3
        case ShaderDataType::Mat4:      return 4; // 4* float4
        case ShaderDataType::Int:       return 1;
        case ShaderDataType::Int2:      return 2;
        case ShaderDataType::Int3:      return 3;
        case ShaderDataType::Int4:      return 4;
        case ShaderDataType::Bool:      return 1;
        case ShaderDataType::Texture2D: return 0;
        case ShaderDataType::None:      return 0;
        default: ERROR("Nooo");
    }
    return 0;
}


std::string ReadFile(const std::string& path)
{
    std::string result;
    std::ifstream in(path, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
    if (in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
        else
        {
            ERROR("Could not read from file %s.", path.data());
        }
    }
    else
    {
        ERROR("Could not open file %s.", path.data());
    }

    return result;
}



class Camera
{
public:
    [[nodiscard]] mat4 ProjectionMatrix() const noexcept
    {
        if (this->is_orthographic)
            return this->OrthographicProjection();
        else
            return this->PerspectiveProjection();
    }

    [[nodiscard]] mat4 OrthographicProjection() const noexcept
    {
        mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, this->near, this->far);
        return projection;
    }

    [[nodiscard]] mat4 PerspectiveProjection() const noexcept
    {
        mat4 projection = glm::perspective(glm::radians(this->fov), this->aspect_ratio, this->near, this->far);
        return projection;
    }

    [[nodiscard]] mat4 ViewMatrix() const noexcept
    {
        mat4 view = glm::lookAt(this->position, this->position + this->forward, WORLD_AXIS_UP);
        return view;
    }
//private:
    vec3 position = vec3(0);

    vec3 forward  = WORLD_AXIS_FORWARD;
    vec3 up       = WORLD_AXIS_UP;
    vec3 right    = WORLD_AXIS_RIGHT;

    double pitch  = 0.0f;
    double yaw    = 0.0f;

    float speed = 4.0f;

    float near = 0.1f;
    float far  = 1000.0f;
    float fov  = 80.0f;
    float aspect_ratio   = 1.0f;
    bool is_orthographic = false;
};


class Window
{
public:
    Window(GLFWwindow* id) : id{id} {}

    static Window Create(int width, int height, const char* name)
    {
        ASSERT(glfwInit(), "Oh no!");

        if (RENDERER_API == RendrererAPI::OpenGL)
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

            if constexpr (DEBUG)
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        }
        else
        {
            ERROR("No renderer API.");
        }

        auto id = glfwCreateWindow(width, height, name, nullptr, nullptr);
        ASSERT(id != nullptr, "Failed to create GLFW window");

        if (RENDERER_API == RendrererAPI::OpenGL)
        {
            glfwMakeContextCurrent(id);
            ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize glad!");

            INFO("OpenGL Info - Vendor: %s, Renderer: %s, Version: %s",
                 glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION)
             );

//            if constexpr (DEBUG)
//                glad_set_post_callback(OpenGLCallback);

            context_created = true;
        }
        else
        {
            ERROR("No renderer API.");
        }

//        glfwSetFramebufferSizeCallback(window.id, FrameBufferSizeCallback);
//        glfwSetKeyCallback(window.id, key_callback);
//        glfwSetCursorPosCallback(window.id, MouseCallback);
//        glfwSetScrollCallback(window.id, ScrollCallback);

        INFO("Successfully created 'GLFWwindow'");
        return { id };
    }

    bool Continue()
    {
        return !glfwWindowShouldClose(this->id);
    }

    void Update()
    {
        glfwPollEvents();
        glfwSwapBuffers(this->id);
    }

private:
    GLFWwindow* id;
};


class BufferElement
{
public:
    std::string      name;
    ShaderDataType   type;
    std::uint32_t    size;
    std::size_t      offset;
    bool normalized;

    BufferElement() = default;

    BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
    {
    }
};

class BufferLayout
{
public:
    BufferLayout() {}

    BufferLayout(std::initializer_list<BufferElement> elements)
            : elements(elements)
    {
        size_t offset = 0;
        this->stride  = 0;
        for (auto& element : this->elements)
        {
            element.offset = offset;
            offset += element.size;
            this->stride += element.size;
        }
    }

    uint32_t GetStride() const { return this->stride; }
    const std::vector<BufferElement>& GetElements() const { return this->elements; }

    std::vector<BufferElement>::iterator begin() { return this->elements.begin(); }
    std::vector<BufferElement>::iterator end()   { return this->elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return this->elements.begin(); }
    std::vector<BufferElement>::const_iterator end()   const { return this->elements.end(); }
private:
    std::vector<BufferElement> elements;
    uint32_t stride = 0;
};


class Shader
{
public:
    struct ValueInfo
    {
        std::size_t    size;
        ShaderDataType type;
        std::size_t    component_count;
        std::int32_t   index;
    };

    struct UniformBufferInfo
    {
        GLint  index;
        GLuint binding;
        std::string name;
    };

    using AttributeMap = std::unordered_map<std::string, ValueInfo>;
    using UniformMap   = std::unordered_map<std::string, ValueInfo>;

    Shader(std::uint32_t id, std::string name, AttributeMap&& attributes, UniformMap&& uniforms) : id{id}, name{name}, attributes{attributes}, uniforms{uniforms} {}

    static Shader Create(const char* name, const char* vertex_source, const char* fragment_source)
    {
        ASSERT(*vertex_source   != 0, "Vertex source cannot be empty");
        ASSERT(*fragment_source != 0, "Fragment source cannot be empty");

        // Allocate, assign and compile the vertex shader.
        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertex_source, nullptr);
        glCompileShader(vertex);

        // Allocate, assign and compile the fragment shader.
        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragment_source, nullptr);
        glCompileShader(fragment);

        // Allocate and attach the compiled shaders. Then link the attached shaders into a program.
        GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);


        // Check shaders compile status and programs link status (and validate status?).
        constexpr int log_size = 512;
        char info_log[log_size];

        GLint success = 0;
        if (!program)
        {
            glGetShaderInfoLog(vertex, log_size, nullptr, info_log);
            WARNING("Invalid shader program for program '%s'.\n%s", name, info_log);
        }
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, log_size, nullptr, info_log);
            WARNING("Vertex shader compilation failed for program '%s'.\n%s", name, info_log);
        }
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, log_size, nullptr, info_log);
            WARNING("Fragment shader compilation failed for program '%s'.\n%s", name, info_log);
        }
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, log_size, nullptr, info_log);
            WARNING("Shader program linking failed for program '%s'.\n%s", name, info_log);
        }


        // ---- QUERY ATTRIBUTES ----
        AttributeMap attributes;
        {
            GLint max_name_length = 0;
            glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_name_length);

            GLchar* buffer = new GLchar[max_name_length];

            GLint   count  = 0;
            GLint   size   = 0;   // Size of the variable. The size is in units of the type, not bytes!
            GLenum  type   = 0;   // Type of the variable (float, vec3 or mat4, etc).
            GLsizei length = 0;   // Numbers of characters written to the buffer.
            glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);

            attributes.reserve(count);
            for (int i = 0; i < count; i++)
            {
                glGetActiveAttrib(program, (GLuint)i, max_name_length, &length, &size, &type, buffer);
                GLint index = glGetAttribLocation(program, buffer);

                auto shader_data_type = OpenGLTypeToShaderDataType(type);

                auto attribute_name = std::string(buffer, length);
                attributes[attribute_name] = ValueInfo {
                        ShaderDataTypeSize(shader_data_type), shader_data_type, GetComponentCount(shader_data_type), index
                };
            }

            delete[] buffer;
        }

        // ---- QUERY UNIFORMS ----
        UniformMap uniforms;
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

                auto shader_data_type = OpenGLTypeToShaderDataType(type);

                auto uniform_name = std::string(buffer, length);
                uniforms[uniform_name] = ValueInfo {
                        ShaderDataTypeSize(shader_data_type), shader_data_type, GetComponentCount(shader_data_type), index
                };
            }

            delete[] buffer;
        }

        // Delete the shaders as they are no longer needed when we have the program (unless we'd like to reuse one, or more,
        // of the shaders to create a new program).
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return { program, name, std::move(attributes), std::move(uniforms) };
    }

    bool Validate(int index, const BufferElement& element) const
    {
        auto it = attributes.find(element.name);
        if (it == attributes.end())
        {
            WARNING("Attribute '%s' doesn't exist in shader '%s'.", element.name.data(), this->name.data());
            return false;
        }

        auto info  = it->second;
        bool valid = true;
        if (info.type != element.type)
        {
            WARNING("Attribute '%s' in shader '%s' has type %i, not %i.", element.name.data(), this->name.data(), info.type, element.type);
            valid = false;
        }
        if (info.size != element.size)
        {
            WARNING("Attribute '%s' in shader '%s' has size %i, not %i.", element.name.data(), this->name.data(), info.size, element.size);
            valid = false;
        }
        if (info.index != index)
        {
            WARNING("Attribute '%s' in shader '%s' has index %i, not %i.", element.name.data(), this->name.data(), info.index, index);
            valid = false;
        }

        return valid;
    }

    const AttributeMap& GetAttributes() const
    {
        return this->attributes;
    }

    const UniformMap& GetUniforms() const
    {
        return this->uniforms;
    }

    static void Destroy(Shader* shader)
    {
        glDeleteProgram(shader->id);
        shader = nullptr;
    }

    void Bind() const
    {
        glUseProgram(this->id);
    }

    void UnBind() const
    {
        glUseProgram(0);
    }

    template <typename T>
    void SetUniform(const char* name, const T& data)
    {

    }

private:
    std::uint32_t id;
    std::string   name;
    AttributeMap  attributes;
    UniformMap    uniforms;
};

class VertexBuffer
{
public:
    VertexBuffer(std::uint32_t id) : id{id} {}

    static VertexBuffer Create(std::size_t size)
    {
        std::uint32_t id;
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

        return { id };
    }
    static VertexBuffer Create(float* vertices, std::size_t size)
    {
        std::uint32_t id;
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

        return { id };
    }

    static void Destroy(VertexBuffer* buffer)
    {
        glDeleteBuffers(1, &buffer->id);
        buffer = nullptr;
    }

    void Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->id);
    }

    void Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void SetData(const void* data, std::uint32_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->id);
        {
            GLint buffer_size = -1;
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
            if (buffer_size <= size)
            {
                WARNING("Buffer of size %d can't fit data of size %d.", buffer_size, size);
                return;
            }
        }
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
    void SetData(const void* data, std::uint32_t from, std::uint32_t to)
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->id);
        glBufferSubData(GL_ARRAY_BUFFER, from, to, data);
    }

    void SetLayout(const BufferLayout& layout)
    {
        this->layout = layout;
    }

    const BufferLayout& GetLayout() const
    {
        return this->layout;
    }

private:
    std::uint32_t id;
    BufferLayout layout;
};

class IndexBuffer
{
public:
    IndexBuffer(std::uint32_t id, std::uint32_t count) : id{id}, count{count} {}

    static IndexBuffer Create(std::size_t size)
    {
        std::uint32_t id;
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

        return { id, 0 };
    }
    static IndexBuffer Create(const std::uint32_t* indices, std::size_t size)
    {
        std::uint32_t id;
        glGenBuffers(1, &id);

        // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
        // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);

        return { id, std::uint32_t(size / sizeof(std::uint32_t)) };
    }

    static void Destroy(IndexBuffer* buffer)
    {
        glDeleteBuffers(1, &buffer->id);
        buffer = nullptr;
    }

    void Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
    }

    void Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    void SetData(const std::uint32_t* data, std::uint32_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
    std::uint32_t GetCount() const
    {
        return this->count;
    }

private:
    std::uint32_t id;
    std::uint32_t count;
};


class VertexArray
{
public:
    VertexArray(std::uint32_t id) : id{id}, index_buffer{}, vertex_buffers{} {}

    static VertexArray Create()
    {
        std::uint32_t id;
        glGenVertexArrays(1, &id);
        return { id };
    }

    static void Destroy(VertexArray* array)
    {
        glDeleteBuffers(1, &array->id);
        array = nullptr;
    }

    void Bind() const
    {
        glBindVertexArray(this->id);
    }

    void Unbind() const
    {
        glBindVertexArray(0);
    }

    void AddVertexBuffer(const Shader& shader, const VertexBuffer& buffer)
    {
        ASSERT(buffer.GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        glBindVertexArray(this->id);
        buffer.Bind();

        auto attributes = shader.GetAttributes();

        const auto& layout = buffer.GetLayout();
        for (const auto& element : layout)
        {
            switch (element.type)
            {
                case ShaderDataType::Float1:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                {
                    auto index = this->index++;
                    bool is_valid = shader.Validate(index, element);
                    attributes.erase(element.name);

                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(index,
                                          GetComponentCount(element.type),
                                          ShaderDataTypeToOpenGLBaseType(element.type),
                                          element.normalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.offset);
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                {
//                    uint8_t count = element.GetComponentCount();
//                    for (uint8_t i = 0; i < count; i++)
//                    {
//                        glEnableVertexAttribArray(m_VertexBufferIndex);
//                        glVertexAttribPointer(m_VertexBufferIndex,
//                                              count,
//                                              ShaderDataTypeToOpenGLBaseType(element.Type),
//                                              element.Normalized ? GL_TRUE : GL_FALSE,
//                                              layout.GetStride(),
//                                              (const void*)(element.Offset + sizeof(float) * count * i));
//                        glVertexAttribDivisor(m_VertexBufferIndex, 1);
//                        m_VertexBufferIndex++;
//                    }
                    break;
                }
                default:
                    ASSERT(false, "Unknown ShaderDataType!");
            }
        }

        if (attributes.size() > 0)
            for (const auto& key : attributes)
                WARNING("Vertex buffer doesn't 'contain' %s!", key.first.data());

        this->vertex_buffers.push_back(buffer);
    }

    void SetIndexBuffer(const IndexBuffer& buffer)
    {
        this->Bind();
        buffer.Bind();

        this->index_buffer = buffer;
    }
    const IndexBuffer& GetIndexBuffer() const
    {
        return this->index_buffer.value();
    }

private:
    std::uint32_t id;
    std::uint32_t index = 0;
    std::optional<IndexBuffer> index_buffer;
    std::vector<VertexBuffer>  vertex_buffers;

};




class Image
{
public:
    static constexpr unsigned char EMPTY_DATA[3] = {0, 0, 0};

    int width     = 0;
    int height    = 0;
    int channels  = 0;
    const std::uint8_t* data = nullptr;

    std::string name      = "";
    std::string directory = "";

    static const Image from_raw(int width, int height, int channels, const unsigned char* data, const std::string& name, const std::string& directory);

    static const Image empty()
    {
        int width     = 1;
        int height    = 1;
        int channels  = 3;
        const unsigned char* data = EMPTY_DATA;

        std::string name      = "empty";
        std::string directory = "";

        return { width, height, channels, data, name, directory };
    }

    // TODO(ted): @LEAK We don't free this.
    static const Image from_path(const std::string& name, const std::string& directory)
    {
        int width, height, channels;
        auto* data = stbi_load((directory + name).data(), &width, &height, &channels, 0);
        if (!data)
            WARNING("Couldn't load %s%s.", directory.data(), name.data());

        return { width, height, channels, data, name, directory };
    }
};




class Texture2D
{
public:

    Texture2D() : id{0} {}

    Texture2D(std::uint32_t id, std::uint32_t width, std::uint32_t height, GLenum target, GLenum data_type, GLenum data_format)
        : id{id}, width{width}, height{height}, target{target}, data_type{data_type}, data_format{data_format} {}

    static Texture2D Create(std::uint32_t width, std::uint32_t height, const std::uint8_t* data)
    {
        std::uint32_t id;

        GLenum target = GL_TEXTURE_2D;
        GLenum data_format = GL_RGBA;
        GLenum data_type   = GL_UNSIGNED_BYTE;
        GLenum internal_format = GL_RGBA;

        glGenTextures(1, &id);
        glBindTexture(target, id);

//        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
//        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL,  1000);  // 1000 is the default.
//        glTexParameterf(target, GL_TEXTURE_LOD_BIAS,   0.0f);

        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(target, 0, internal_format, width, height, 0, data_format, data_type, data);

        return { id, width, height, target, data_type, data_format };
    }


    static Texture2D Create(const Image& image)
    {
        std::uint32_t id;

        GLenum target = GL_TEXTURE_2D;
        GLenum data_type = GL_UNSIGNED_BYTE;
        GLenum internal_format = GL_RGBA;

        GLenum data_format;
        if (image.channels == 4)
            data_format = GL_RGBA;
        else if (image.channels == 3)
            data_format = GL_RGB;
        else
            ERROR("Don't support image with %i channels.", image.channels);

        glGenTextures(1, &id);
        glBindTexture(target, id);

//        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
//        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL,  1000);  // 1000 is the default.
//        glTexParameterf(target, GL_TEXTURE_LOD_BIAS,   0.0f);

        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(target, 0, internal_format, image.width, image.height, 0, data_format, data_type, image.data);

        return { id, std::uint32_t(image.width), std::uint32_t(image.height), target, data_type, data_format };
    }

    static void Destroy(Texture2D* texture)
    {
        glDeleteTextures(1, &texture->id);
        texture = nullptr;
    }

    void SetData(const std::uint8_t* data)
    {
        // this->dirty = true;
        // memcpy(this->data, data, this->width * this->height * sizeof(std::uint8_t));
        ASSERT(this->data_type == GL_UNSIGNED_BYTE, "Wrong data type!");

        glBindTexture(this->target, this->id);
        glTexSubImage2D(this->target, 0, 0, 0, this->width, this->height, this->data_format, this->data_type, data);
    }

    void Bind(std::uint32_t unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(this->target, this->id);

        // if (this->dirty)
        // {
        //     glTexSubImage2D(this->target, 0, 0, 0, this->width, this->height, this->data_format, this->data_type, data);
        //     this->dirty = false;
        // }
    }

//private:
    std::uint32_t id;
    std::uint32_t width;
    std::uint32_t height;
    GLenum target;
    GLenum data_type;
    GLenum data_format;
};


struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv_coord;
    glm::vec3 normal;
    float texture_index;
};

// newmtl name
//    defines the name of the material.
// Ka r g b
//    defines the ambient color of the material to be (r,g,b). The default is (0.2,0.2,0.2);
// Kd r g b
//     defines the diffuse color of the material to be (r,g,b). The default is (0.8,0.8,0.8);
// Ks r g b
//     defines the specular color of the material to be (r,g,b). This color shows up in highlights. The default is (1.0,1.0,1.0);
// d alpha
//     defines the non-transparency of the material to be alpha. The default is 1.0 (not transparent at all). The quantities d and Tr are the opposites of each other, and specifying transparency or nontransparency is simply a matter of user convenience.
// Tr alpha
//     defines the transparency of the material to be alpha. The default is 0.0 (not transparent at all). The quantities d and Tr are the opposites of each other, and specifying transparency or nontransparency is simply a matter of user convenience.
// Ns s
//     defines the shininess of the material to be s. The default is 0.0;
// illum n
//     denotes the illumination model used by the material. illum = 1 indicates a flat material with no specular highlights, so the value of Ks is not used. illum = 2 denotes the presence of specular highlights, and so a specification for Ks is required.
// map_Ka filename
//    names a file containing a texture map, which should just be an ASCII dump of RGB values;
struct SoftwareMaterial
{
    enum Illumination
    {
        NO_SPECULAR = 1, HAS_SPECULAR
    };

    std::string name = "default";      // newmtl

    vec3 ambient  = {0.2, 0.2, 0.2};   // Ka
    vec3 diffuse  = {0.8, 0.8, 0.8};   // Kd
    vec3 specular = {1.0, 1.0, 1.0};   // Ks

    float shininess     = 0.0;         // NS
    float transparency  = 0.0;         // Tr
    float opaqueness    = 1.0;         // d

    Illumination illumination = NO_SPECULAR;  // illum

    std::optional<Image> ambient_map  = {};  // map_Ka
    std::optional<Image> diffuse_map  = {};  // map_Kd
    std::optional<Image> specular_map = {};  // map_Ks
    std::optional<Image> bump_map     = {};  // map_bump

    std::optional<Image> opaque_map   = {};  // map_d
};

struct SoftwareMesh
{
    std::size_t          mesh_id     = 0;
    std::size_t          material_id = 0;
    std::string          name        = "";
    std::vector<Vertex>  vertices    = {};
    SoftwareMaterial*    material    = nullptr;
};


// Stolen from SO. Should be temporary.
struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;
    }
};

// TODO(ted): Stupidly slow and probably buggy.
std::pair<std::vector<SoftwareMesh>, std::vector<SoftwareMaterial>>
LoadScene(const std::string& input_file, const std::string& material_directory)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = material_directory;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(input_file, reader_config))
        ASSERT(!reader.Error().empty(), reader.Error().data());

    if (!reader.Warning().empty())
        WARNING(reader.Warning().data());

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();


    std::vector<SoftwareMaterial> all_materials;
    all_materials.reserve(materials.size());
    for (const auto& material : materials)
        all_materials.emplace_back(SoftwareMaterial {
                std::move(material.name),
                { material.ambient[0],  material.ambient[1],  material.ambient[2]  },
                { material.diffuse[0],  material.diffuse[1],  material.diffuse[2]  },
                { material.specular[0], material.specular[1], material.specular[2] },
                material.shininess, 0.0, 1.0,
                (material.illum == 1) ? SoftwareMaterial::NO_SPECULAR : SoftwareMaterial::HAS_SPECULAR,
                (material.ambient_texname.size()  > 0) ? std::move(Image::from_path(material.ambient_texname,  material_directory)) : std::optional<Image>(),
                (material.diffuse_texname.size()  > 0) ? std::move(Image::from_path(material.diffuse_texname,  material_directory)) : std::optional<Image>(),
                (material.specular_texname.size() > 0) ? std::move(Image::from_path(material.specular_texname, material_directory)) : std::optional<Image>(),
                (material.bump_texname.size()     > 0) ? std::move(Image::from_path(material.bump_texname,     material_directory)) : std::optional<Image>(),
                (material.alpha_texname.size()    > 0) ? std::move(Image::from_path(material.alpha_texname,    material_directory)) : std::optional<Image>()
        });



    std::unordered_map<std::pair<std::size_t, std::size_t>, SoftwareMesh, pair_hash> meshes;
    for (size_t s = 0; s < shapes.size(); s++)
    {
        std::vector<Vertex> vertices;
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            auto m  = shapes[s].mesh.material_ids[f];  // per-face material
            auto i  = std::pair(std::size_t(s), std::size_t(m));
            auto it = meshes.find(i);
            if (it == meshes.end())
            {
                meshes[i] = std::move(SoftwareMesh {
                        i.first, i.second, shapes[s].name, {}, &all_materials[m]
                });
            };

            auto& mesh = meshes[i];

            int fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                mesh.vertices.push_back({
                                                {vx, vy, vz}, {tx, ty}, {nx, ny, nz}
                                        });
            }
            index_offset += fv;
        }
    }

    std::vector<SoftwareMesh> all_meshes;
    all_meshes.reserve(meshes.size());

    for(auto it : meshes)
        all_meshes.push_back(it.second);

    return { all_meshes, all_materials };
}


class Renderer2D
{
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 uv_coord;
        glm::vec3 normal;
        float texture_index;
    };
    using Index = std::uint32_t;

    enum Color { Default = 0, Red, Green, Blue };

    Renderer2D(VertexArray vertex_array, VertexBuffer vertex_buffer,
             Shader shader, Index* quad_index, Vertex* quad_vertex, Texture2D* textures, std::size_t texture_count)
            : vertex_array{vertex_array},
              vertex_buffer{vertex_buffer},
              shader{shader},
              quad_count{0},
              quad_indices{quad_index},
              quad_vertex{quad_vertex},
              texture_count{texture_count},
              textures{textures}
    {}

    static constexpr std::size_t MAX_QUADS = 1000;
    static constexpr std::size_t MAX_VERTICES = MAX_QUADS * sizeof(Vertex);
    static constexpr std::size_t MAX_INDICES  = MAX_QUADS * 6;
    static constexpr std::size_t MAX_TEXTURES = 16; // TODO: RenderCaps

    static Renderer2D Create()
    {
        ASSERT(context_created, "No.");

        Vertex*    quad_vertices = new Vertex[MAX_VERTICES]{ };
        Index*     quad_indices  = new Index[MAX_INDICES]{ };
        Texture2D* textures      = new Texture2D[MAX_TEXTURES]{ };

        auto color_shader = Shader::Create("color_shader",
             ReadFile("../resources/shaders/batch.vs.glsl").data(),
             ReadFile("../resources/shaders/batch.fs.glsl").data()
         );

        auto vertex_buffer = VertexBuffer::Create(MAX_VERTICES);
        vertex_buffer.SetLayout({
            { ShaderDataType::Float3, "position" },
            { ShaderDataType::Float2, "uv_coord" },
            { ShaderDataType::Float3, "normal" },
            { ShaderDataType::Float1, "texture_index" },
        });

        uint32_t offset = 0;
        for (uint32_t i = 0; i < MAX_INDICES; i += 6)
        {
            quad_indices[i + 0] = offset + 0;
            quad_indices[i + 1] = offset + 1;
            quad_indices[i + 2] = offset + 2;

            quad_indices[i + 3] = offset + 2;
            quad_indices[i + 4] = offset + 3;
            quad_indices[i + 5] = offset + 0;

            offset += 4;
        }
        auto index_buffer = IndexBuffer::Create(quad_indices, MAX_INDICES);

        auto vertex_array = VertexArray::Create();
        vertex_array.AddVertexBuffer(color_shader, vertex_buffer);
        vertex_array.SetIndexBuffer(index_buffer);

        const std::uint8_t pink[] = {255, 0, 255, 0};
        auto default_texture = Texture2D::Create(1, 1, pink);

        const std::uint8_t red[] = {255, 0, 0, 255};
        auto texture0 = Texture2D::Create(1, 1, red);

        const std::uint8_t green[] = {0, 255, 0, 255};
        auto texture1 = Texture2D::Create(1, 1, green);

        const std::uint8_t blue[] = {0, 0, 255, 255};
        auto texture2 = Texture2D::Create(1, 1, blue);

        textures[Color::Default] = default_texture;
        textures[Color::Red]     = texture0;
        textures[Color::Green]   = texture1;
        textures[Color::Blue]    = texture2;
        std::size_t texture_count  = 4;

        return {vertex_array, vertex_buffer, color_shader, quad_indices, quad_vertices, textures, texture_count };
    }

    void BeginScene(const Camera& camera)
    {
        this->view_matrix = camera.ViewMatrix();
        this->proj_matrix = camera.ProjectionMatrix();
        this->quad_count  = 0;
    }
    void DrawQuad(vec3 position, float scale, const Image& image)
    {
        auto it = this->images.find(image.name);
        if (it == this->images.end())
        {
            auto texture = Texture2D::Create(image);
            this->textures[this->texture_count] = texture;
            this->images[image.name] = this->texture_count;
            this->DrawQuad(position, scale, this->texture_count);
            ++this->texture_count;
        }
        else
        {
            this->DrawQuad(position, scale, it->second);
        }
    }
    void DrawQuad(vec3 position, float scale, int texture)
    {
//        if (this->quad_count >= this->MAX_QUADS)
//            NextBatch();

        constexpr vec2 quad_uv_coords[] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };
        vec3 quad_vertices[] = {
            vec3{ -0.4f, -0.4f, 0.0f },
            vec3{  0.4f, -0.4f, 0.0f },
            vec3{  0.4f,  0.4f, 0.0f },
            vec3{ -0.4f,  0.4f, 0.0f }
        };

        for (int i = 0; i < 4; i++)
        {
            auto& vertex = this->quad_vertex[this->quad_count * 4 + i];
            vertex.position = quad_vertices[i] + position;
            vertex.uv_coord = quad_uv_coords[i];
            vertex.normal   = vec3{0.5f};
            vertex.texture_index = float(texture);
        }
        ++this->quad_count;
    }
    void EndScene()
    {
        this->Flush();
    }
    void Flush()
    {
        if (this->quad_count == 0)
            return;

        this->shader.Bind();
        this->shader.SetUniform("view",       this->view_matrix);
        this->shader.SetUniform("projection", this->proj_matrix);

        this->vertex_buffer.SetData(this->quad_vertex, this->quad_count * 4 * sizeof(Vertex));
        this->vertex_buffer.Unbind();

        for (int i = 0; i < this->texture_count; i++)
        {
            // https://www.khronos.org/opengl/wiki/Array_Texture
            auto texture  = std::string("texture") + std::to_string(i);
            auto location = this->shader.GetUniforms().at(texture).index;
            this->textures[i].Bind(i);
            glUniform1i(location, i);
        }

        this->vertex_array.Bind();
        glDrawElements(GL_TRIANGLES, this->quad_count * 12, GL_UNSIGNED_INT, nullptr);

        this->quad_count = 0;
    }

private:
    VertexArray  vertex_array;
    VertexBuffer vertex_buffer;
    Shader       shader;

    std::size_t  quad_count;
    Index*       quad_indices;
    Vertex*      quad_vertex;

    std::size_t  texture_count;
    Texture2D*   textures;

    mat4 view_matrix {};
    mat4 proj_matrix {};

    std::unordered_map<std::string, std::uint32_t> images;
};



class Renderer3D
{
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 uv_coord;
        glm::vec3 normal;
    };
    using Index = std::uint32_t;

    Renderer3D(Shader shader) : shader{shader} {}

    static constexpr std::size_t MAX_VERTICES = 32768;
    static constexpr std::size_t MAX_INDICES  = 1024 * 6;
    static constexpr std::size_t MAX_TEXTURES = 16; // TODO: RenderCaps

    static Renderer3D Create()
    {
        ASSERT(context_created, "No.");

        auto mesh_shader = Shader::Create("mesh_shader",
            ReadFile("../resources/shaders/basic.vs.glsl").data(),
            ReadFile("../resources/shaders/basic.fs.glsl").data()
        );

        return { mesh_shader };
    }

    void BeginScene(const Camera& camera)
    {
        this->view_matrix = camera.ViewMatrix();
        this->proj_matrix = camera.ProjectionMatrix();
    }
    void DrawMeshes(const std::vector<SoftwareMesh>& meshes, const std::vector<SoftwareMaterial>& materials)
    {
        const auto empty_texture = Texture2D::Create(Image::empty());

        for (const auto& data : meshes)
        {
            std::string name =
                    (data.material && data.material->diffuse_map) ?
                    data.material->name :
                    "Unnamed";

            Texture2D texture =
                    (data.material && data.material->diffuse_map) ?
                    Texture2D::Create(data.material->diffuse_map.value()) :
                    empty_texture;   // TODO(ted): Colored material;

            std::vector<Vertex> new_vertices;
            for (const auto& mesh_vertex : data.vertices)
            {
                new_vertices.emplace_back(
                    Vertex{
                        mesh_vertex.position,
                        mesh_vertex.uv_coord,
                        mesh_vertex.normal
                    }
                );
            }

            auto vertex_buffer = VertexBuffer::Create(2 * new_vertices.size() * sizeof(Vertex));
            vertex_buffer.SetLayout({
                    { ShaderDataType::Float3, "position" },
                    { ShaderDataType::Float2, "uv_coord" },
                    { ShaderDataType::Float3, "normal" },
            });
            auto vertex_array = VertexArray::Create();
            vertex_array.AddVertexBuffer(this->shader, vertex_buffer);

            this->render_data.emplace(name, RenderData {
                std::move(new_vertices),
                std::move(texture),
                vertex_array,
                vertex_buffer
            });
        }
    }
    void EndScene()
    {
        this->Flush();
    }
    void Flush()
    {
        this->shader.Bind();

        auto model = glm::mat4();
        model = glm::translate(model, vec3(0));
        this->shader.SetUniform("model",      model);
        this->shader.SetUniform("view",       this->view_matrix);
        this->shader.SetUniform("projection", this->proj_matrix);

        for (auto& [name, render_data] : this->render_data)
        {
//            render_data.vertex_buffer.SetData(render_data.vertices.data(), render_data.vertices.size() * sizeof(Vertex));

            const auto& vertices = render_data.vertices;

            GLuint vao;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Create vertex buffer to put our data into video memory.
            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            // Tell OpenGL the data's format.
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv_coord));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));


            auto texture  = std::string("diffuse");
            auto location = this->shader.GetUniforms().at(texture).index;
            render_data.texture.Bind(0);
            glUniform1i(location, 0);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        }
    }

private:
    struct RenderData
    {
        std::vector<Vertex> vertices;
        Texture2D texture;

        VertexArray  vertex_array;
        VertexBuffer vertex_buffer;
    };


    Shader shader;

    std::vector<Vertex>    vertices {};
    std::vector<Index>     indices  {};
    std::vector<Texture2D> textures {};

    mat4 view_matrix {};
    mat4 proj_matrix {};

    std::unordered_map<std::string, RenderData> render_data {};
};





int main()
{
    stbi_set_flip_vertically_on_load(true);

    auto window      = Window::Create(2880, 1710, "Game");
//    auto renderer_2d = Renderer2D::Create();
    auto renderer_3d = Renderer3D::Create();

    Camera camera;
    camera.position = vec3{0, 2.0f, 3.0f};
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

//    auto [all_meshes, all_materials] = LoadScene("../resources/models/sponza/sponza.obj", "../resources/models/sponza/");
    auto [all_meshes, all_materials] = LoadScene("../resources/models/cube.obj", "../resources/models/");

    renderer_3d.DrawMeshes(all_meshes, all_materials);
    while (window.Continue())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        renderer_3d.BeginScene(camera);
        renderer_3d.EndScene();
//        renderer_2d.DrawQuad(vec3{ -0.5f,  0.5f, 0.0f }, 0.1f, image1);
//        renderer_2d.DrawQuad(vec3{  0.5f,  0.5f, 0.0f }, 0.1f, image2);
//        renderer_2d.DrawQuad(vec3{ -0.5f, -0.5f, 0.0f }, 0.1f, Renderer::Color::Blue);
//        renderer_2d.DrawQuad(vec3{  0.5f, -0.5f, 0.0f }, 0.1f, Renderer::Color::Green);

        window.Update();
    }
}



