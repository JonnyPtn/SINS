////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Err.hpp>
#include <fstream>
#include <vector>

#include <bgfx/bgfx.h>

namespace
{
    // Read the contents of a file into an array of char
    bool getFileContents(const std::string& filename, std::vector<char>& buffer)
    {
        std::ifstream file(filename.c_str(), std::ios_base::binary);
        if (file)
        {
            file.seekg(0, std::ios_base::end);
            std::streamsize size = file.tellg();
            if (size > 0)
            {
                file.seekg(0, std::ios_base::beg);
                buffer.resize(static_cast<std::size_t>(size));
                file.read(buffer.data(), size);
            }
            buffer.push_back('\0');
            return true;
        }
        else
        {
            return false;
        }
    }

    // Read the contents of a stream into an array of char
    bool getStreamContents(sf::InputStream& stream, std::vector<char>& buffer)
    {
        bool success = true;
        sf::Int64 size = stream.getSize();
        if (size > 0)
        {
            buffer.resize(static_cast<std::size_t>(size));
            stream.seek(0);
            sf::Int64 read = stream.read(buffer.data(), size);
            success = (read == size);
        }
        buffer.push_back('\0');
        return success;
    }

    // Transforms an array of 2D vectors into a contiguous array of scalars
    template <typename T>
    std::vector<T> flatten(const sf::Vector2<T>* vectorArray, std::size_t length)
    {
        const std::size_t vectorSize = 2;

        std::vector<T> contiguous(vectorSize * length);
        for (std::size_t i = 0; i < length; ++i)
        {
            contiguous[vectorSize * i]     = vectorArray[i].x;
            contiguous[vectorSize * i + 1] = vectorArray[i].y;
        }

        return contiguous;
    }

    // Transforms an array of 3D vectors into a contiguous array of scalars
    template <typename T>
    std::vector<T> flatten(const sf::Vector3<T>* vectorArray, std::size_t length)
    {
        const std::size_t vectorSize = 3;

        std::vector<T> contiguous(vectorSize * length);
        for (std::size_t i = 0; i < length; ++i)
        {
            contiguous[vectorSize * i]     = vectorArray[i].x;
            contiguous[vectorSize * i + 1] = vectorArray[i].y;
            contiguous[vectorSize * i + 2] = vectorArray[i].z;
        }

        return contiguous;
    }

    // Transforms an array of 4D vectors into a contiguous array of scalars
    std::vector<float> flatten(const sf::Color* vectorArray, std::size_t length)
    {
        const std::size_t vectorSize = 4;

        std::vector<float> contiguous(vectorSize * length);
        for (std::size_t i = 0; i < length; ++i)
        {
            contiguous[vectorSize * i]     = vectorArray[i].r;
            contiguous[vectorSize * i + 1] = vectorArray[i].g;
            contiguous[vectorSize * i + 2] = vectorArray[i].b;
            contiguous[vectorSize * i + 3] = vectorArray[i].a;
        }

        return contiguous;
    }
}


namespace sf
{
////////////////////////////////////////////////////////////
Shader::CurrentTextureType Shader::CurrentTexture;


////////////////////////////////////////////////////////////
Shader::Shader() :
m_shaderProgram (0),
m_currentTexture(-1),
m_textures      (),
m_uniforms      ()
{
}


////////////////////////////////////////////////////////////
Shader::~Shader()
{

    // Destroy effect program
    if (m_shaderProgram)
        bgfx::destroy(bgfx::ShaderHandle{ m_shaderProgram });
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::string& filename, Type type)
{
    // Read the file
    std::vector<char> shader;
    if (!getFileContents(filename, shader))
    {
        err() << "Failed to open shader file \"" << filename << "\"" << std::endl;
        return false;
    }

    // Compile the shader program
    if (type == Type::Vertex)
        return compile(shader.data(), nullptr, nullptr);
    else if (type == Type::Geometry)
        return compile(nullptr, shader.data(), nullptr);
    else
        return compile(nullptr, nullptr, shader.data());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
    // Read the vertex shader file
    std::vector<char> vertexShader;
    if (!getFileContents(vertexShaderFilename, vertexShader))
    {
        err() << "Failed to open vertex shader file \"" << vertexShaderFilename << "\"" << std::endl;
        return false;
    }

    // Read the fragment shader file
    std::vector<char> fragmentShader;
    if (!getFileContents(fragmentShaderFilename, fragmentShader))
    {
        err() << "Failed to open fragment shader file \"" << fragmentShaderFilename << "\"" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile(vertexShader.data(), nullptr, fragmentShader.data());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::string& vertexShaderFilename, const std::string& geometryShaderFilename, const std::string& fragmentShaderFilename)
{
    // Read the vertex shader file
    std::vector<char> vertexShader;
    if (!getFileContents(vertexShaderFilename, vertexShader))
    {
        err() << "Failed to open vertex shader file \"" << vertexShaderFilename << "\"" << std::endl;
        return false;
    }

    // Read the geometry shader file
    std::vector<char> geometryShader;
    if (!getFileContents(geometryShaderFilename, geometryShader))
    {
        err() << "Failed to open geometry shader file \"" << geometryShaderFilename << "\"" << std::endl;
        return false;
    }

    // Read the fragment shader file
    std::vector<char> fragmentShader;
    if (!getFileContents(fragmentShaderFilename, fragmentShader))
    {
        err() << "Failed to open fragment shader file \"" << fragmentShaderFilename << "\"" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile(vertexShader.data(), geometryShader.data(), fragmentShader.data());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(const std::string& shader, Type type)
{
    // Compile the shader program
    if (type == Type::Vertex)
        return compile(shader.c_str(), nullptr, nullptr);
    else if (type == Type::Geometry)
        return compile(nullptr, shader.c_str(), nullptr);
    else
        return compile(nullptr, nullptr, shader.c_str());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(const std::string& vertexShader, const std::string& fragmentShader)
{
    // Compile the shader program
    return compile(vertexShader.c_str(), nullptr, fragmentShader.c_str());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
{
    // Compile the shader program
    return compile(vertexShader.c_str(), geometryShader.c_str(), fragmentShader.c_str());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& stream, Type type)
{
    // Read the shader code from the stream
    std::vector<char> shader;
    if (!getStreamContents(stream, shader))
    {
        err() << "Failed to read shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    if (type == Type::Vertex)
        return compile(shader.data(), nullptr, nullptr);
    else if (type == Type::Geometry)
        return compile(nullptr, shader.data(), nullptr);
    else
        return compile(nullptr, nullptr, shader.data());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream)
{
    // Read the vertex shader code from the stream
    std::vector<char> vertexShader;
    if (!getStreamContents(vertexShaderStream, vertexShader))
    {
        err() << "Failed to read vertex shader from stream" << std::endl;
        return false;
    }

    // Read the fragment shader code from the stream
    std::vector<char> fragmentShader;
    if (!getStreamContents(fragmentShaderStream, fragmentShader))
    {
        err() << "Failed to read fragment shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile(vertexShader.data(), nullptr, fragmentShader.data());
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& geometryShaderStream, InputStream& fragmentShaderStream)
{
    // Read the vertex shader code from the stream
    std::vector<char> vertexShader;
    if (!getStreamContents(vertexShaderStream, vertexShader))
    {
        err() << "Failed to read vertex shader from stream" << std::endl;
        return false;
    }

    // Read the geometry shader code from the stream
    std::vector<char> geometryShader;
    if (!getStreamContents(geometryShaderStream, geometryShader))
    {
        err() << "Failed to read geometry shader from stream" << std::endl;
        return false;
    }

    // Read the fragment shader code from the stream
    std::vector<char> fragmentShader;
    if (!getStreamContents(fragmentShaderStream, fragmentShader))
    {
        err() << "Failed to read fragment shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile(vertexShader.data(), geometryShader.data(), fragmentShader.data());
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, float x)
{
    // Don't see a float uniform type?! should it be an array with a single element?
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Int1);
    //bgfx::setUniform(uniform, &x);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, const sf::Vector2f& v)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    //bgfx::setUniform(uniform, &v);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, const sf::Vector3f& v)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    //bgfx::setUniform(uniform, &v);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, const sf::Color& v)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    //bgfx::setUniform(uniform, &v);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, int x)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    //bgfx::setUniform(uniform, &x);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, const sf::Vector2i& v)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    //bgfx::setUniform(uniform, &v);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, const sf::Vector3i& v)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    bgfx::setUniform(uniform, &v);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, bool x)
{
    setUniform(name, static_cast<int>(x));
}


////////////////////////////////////////////////////////////
//void Shader::setUniform(const std::string& name, const Glsl::Bvec2& v)
//{
//    setUniform(name, Glsl::Ivec2(v));
//}
//
//
//////////////////////////////////////////////////////////////
//void Shader::setUniform(const std::string& name, const Glsl::Bvec3& v)
//{
//    setUniform(name, Glsl::Ivec3(v));
//}
//
//
//////////////////////////////////////////////////////////////
//void Shader::setUniform(const std::string& name, const Glsl::Bvec4& v)
//{
//    setUniform(name, Glsl::Ivec4(v));
//}


////////////////////////////////////////////////////////////
//void Shader::setUniform(const std::string& name, const Glsl::Mat3& matrix)
//{
//    UniformBinder binder(*this, name);
//    if (binder.location != -1)
//        glCheck(GLEXT_glUniformMatrix3fv(binder.location, 1, GL_FALSE, matrix.array));
//}
//
//
//////////////////////////////////////////////////////////////
//void Shader::setUniform(const std::string& name, const Glsl::Mat4& matrix)
//{
//    UniformBinder binder(*this, name);
//    if (binder.location != -1)
//        glCheck(GLEXT_glUniformMatrix4fv(binder.location, 1, GL_FALSE, matrix.array));
//}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, const Texture& texture)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Int1);
    auto handle = texture.getNativeHandle();
    //bgfx::setUniform(uniform, &handle);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, CurrentTextureType)
{
    if (m_shaderProgram)
    {
        // Find the location of the variable in the shader
        m_currentTexture = getUniformLocation(name);
    }
}


////////////////////////////////////////////////////////////
void Shader::setUniformArray(const std::string& name, const float* scalarArray, std::size_t length)
{
    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    bgfx::setUniform(uniform, &scalarArray, length);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniformArray(const std::string& name, const sf::Vector2f* vectorArray, std::size_t length)
{
    std::vector<float> contiguous = flatten(vectorArray, length);

    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    bgfx::setUniform(uniform, contiguous.data(), length);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniformArray(const std::string& name, const sf::Vector3f* vectorArray, std::size_t length)
{
    std::vector<float> contiguous = flatten(vectorArray, length);

    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    bgfx::setUniform(uniform, contiguous.data(), length);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
void Shader::setUniformArray(const std::string& name, const sf::Color* vectorArray, std::size_t length)
{
    std::vector<float> contiguous = flatten(vectorArray, length);

    // Kind of guessing the type here
    auto uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4);
    bgfx::setUniform(uniform, contiguous.data(), length);

    // TODO really need to delete the uniforms here, but... not sure when
}


////////////////////////////////////////////////////////////
//void Shader::setUniformArray(const std::string& name, const Glsl::Mat3* matrixArray, std::size_t length)
//{
//    const std::size_t matrixSize = 3 * 3;
//
//    std::vector<float> contiguous(matrixSize * length);
//    for (std::size_t i = 0; i < length; ++i)
//        priv::copyMatrix(matrixArray[i].array, matrixSize, &contiguous[matrixSize * i]);
//
//    UniformBinder binder(*this, name);
//    if (binder.location != -1)
//        glCheck(GLEXT_glUniformMatrix3fv(binder.location, static_cast<GLsizei>(length), GL_FALSE, contiguous.data()));
//}
//
//
//////////////////////////////////////////////////////////////
//void Shader::setUniformArray(const std::string& name, const Glsl::Mat4* matrixArray, std::size_t length)
//{
//    const std::size_t matrixSize = 4 * 4;
//
//    std::vector<float> contiguous(matrixSize * length);
//    for (std::size_t i = 0; i < length; ++i)
//        priv::copyMatrix(matrixArray[i].array, matrixSize, &contiguous[matrixSize * i]);
//
//    UniformBinder binder(*this, name);
//    if (binder.location != -1)
//        glCheck(GLEXT_glUniformMatrix4fv(binder.location, static_cast<GLsizei>(length), GL_FALSE, contiguous.data()));
//}


////////////////////////////////////////////////////////////
unsigned int Shader::getNativeHandle() const
{
    return m_shaderProgram;
}


////////////////////////////////////////////////////////////
void Shader::bind(const Shader* shader)
{
    //TODO: Needed?
}


////////////////////////////////////////////////////////////
bool Shader::isAvailable()
{
    // Not really sure which value to check here?!
    auto caps = bgfx::getCaps();

    return true;
}

////////////////////////////////////////////////////////////
bool Shader::compile(const char* vertexShaderCode, const char* geometryShaderCode, const char* fragmentShaderCode)
{
   //TODO: bgfx shaders are meant to be precompiled, but also see https://github.com/bkaradzic/bgfx/issues/156

    return true;
}


////////////////////////////////////////////////////////////
void Shader::bindTextures() const
{
    // TODO: Needed?
}


////////////////////////////////////////////////////////////
int Shader::getUniformLocation(const std::string& name)
{
    // TODO: Needed?
    return 0;
}

} // namespace sf
