#include "glad/glad.h"
#include "GfxModel.h" 

namespace gfx
{

class UniformSetter {
public:

    template <typename T>
    void set(GLuint program, const std::string& name, const T& value);

private:

    GLuint get_uniform(GLuint, const std::string&);

    std::unordered_map<std::string,GLuint> uniforms_;
};

template <> void UniformSetter::template set(GLuint, const std::string&, const calc::Vec2&);
template <> void UniformSetter::template set(GLuint, const std::string&, const calc::Vec3&);
template <> void UniformSetter::template set(GLuint, const std::string&, const calc::Vec4&);
template <> void UniformSetter::template set(GLuint, const std::string&, const calc::Mat4&);
template <> void UniformSetter::template set(GLuint, const std::string&, const GLint&);
template <> void UniformSetter::template set(GLuint, const std::string&, const GLfloat&);
template <> void UniformSetter::template set(GLuint, const std::string&, const GLuint&);


class Shader {
public:

protected:


    template<typename T>
    GLuint set_uniform(GLuint program, const std::string& name, const T& value)
    {
        uniform_setter_.set(program, name, value);
    }

    GLuint load_texture(const TexInfo& info)
    {
        return texture_loader_.load(info);
    }

private:

    static UniformSetter uniform_setter_;
    static TextureLoader texture_loader_;
}

GLuint create_glsl_program(
    const std::unordered_map<std::string, std::string> & symbols, 
    const std::string& filepath);

}

