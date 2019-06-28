
#ifndef GFX_SHADER_H
#define GFX_SHADER_H

#ifndef _ANDROID_
#include "glad/glad.h"
#endif

#include "GfxModel.h" 

namespace gfx
{

class TexInfo {
public:
	TexInfo(
		std::string path, 
		GLint mag_filter = GL_LINEAR, 
		GLint min_filter = GL_LINEAR_MIPMAP_LINEAR,
		GLsizei num_mipmaps = 0);

	std::string path;
	GLint mag_filter;
	GLint min_filter;
	GLsizei num_mipmaps;
};

}

template <>
struct std::hash<gfx::TexInfo> {
    std::size_t operator()(const gfx::TexInfo& vert) const
    {
		auto unsigned_hasher = std::hash<unsigned>();
        std::size_t seed = 0xdeadbeefc01dbeaf;
        seed = calc::hash_combine(seed, vert.path);
        seed = calc::hash_combine(seed, vert.mag_filter);
        seed = calc::hash_combine(seed, vert.min_filter);
        seed = calc::hash_combine(seed, vert.num_mipmaps);

        return seed;
    }
};

namespace gfx
{

class TextureLoader {
public:

	TextureLoader() {}

	GLuint load(const TexInfo& info);

private:

	std::vector<std::string> fmts_{".tga", ".png", ".jpg", ".bmp"};
	std::unordered_map<TexInfo, GLuint> texs_;
	std::unordered_map<std::string, std::string> rawimgs_;
};

}

namespace gfx
{

class UniformSetter {
public:

    template <typename T>
	void set(GLuint program, const std::string& name, const T& value) {}

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
    void set_uniform(GLuint program, const std::string& name, const T& value)
    {
        uniform_setter_.set(program, name, value);
    }

    GLuint load_texture(const TexInfo& info);

private:

    static UniformSetter uniform_setter_;
    static TextureLoader texture_loader_;
};

GLuint create_glsl_program(
    const std::unordered_map<std::string, std::string> & symbols, 
    const std::string& filepath);

}


#endif
