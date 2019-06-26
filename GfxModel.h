
#ifndef GFX_MODEL_H
#define GFX_MODEL_H

#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "calc.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace gfx
{

namespace vertex_attrib
{

enum VertexAttrib {
	Pos = 1u << 0, 
	Norm = 1u << 1,
	UV = 1u << 2,
	Tan = 1u << 3, 
	Bitan = 1u << 4
};

using AttribCode = unsigned;

constexpr AttribCode PosNormUV = Pos | Norm | UV;
constexpr AttribCode PosTan = Pos | Tan;

}

using vertex_attrib::AttribCode;

class Model;
class Mesh {
public:

	Mesh(const Model& model);
	~Mesh();
	GLuint vao() { return vao_; }

private:
	GLuint vao_ = 0;
	GLuint pos_ = 0;
	GLuint norm_ = 0;
	GLuint uv_ = 0;
	GLuint tan_ = 0;
	GLuint bitan_ = 0;
	GLuint ebo_ = 0;
};

class Material {
public:
	calc::Vec3 ambient;
	calc::Vec3 diffuse;
	calc::Vec3 specular;

	std::string ambient_texpath; // map_Ka
	std::string diffuse_texpath; // map_Kd
	std::string specular_texpath; // map_Ks
	std::string bump_texpath;    // map_bump, map_Bump, bump
	std::string alpha_texpath;   // map_d
};

////
// This is a unified model for both 
// .ind and .obj file descriptions.
// To indicate which set of attributes 
// available, we use AttribCode as a
// bit mask.
////

enum class ModelType {TriangleMesh, Hair};

class Model {
public:

	ModelType model_type();

	// Bind/Unbind vao and enable/disable attribs
	void init_mesh();
	void bind_mesh();
	void unbind_mesh();

	int num_verts();
	int num_parts();
	const Material& material(int part_idx);
	void draw(int part_idx);

	static Model load_from_obj_file(const std::string& inputfile, 
		AttribCode acode = vertex_attrib::PosNormUV,
		calc::Box3D placement = {{0,0,0},{-1,-1,-1}});

	static Model load_from_ind_file(const std::string& inputfile, 
		AttribCode acode = vertex_attrib::PosTan,
		calc::Box3D placement = {{0,0,0},{-1,-1,-1}});
	
private:

	Model() {};

	friend Mesh;

	std::vector<calc::Vec3> positions_;
	std::vector<calc::Vec3> normals_;
	std::vector<calc::Vec2> uvs_;
	std::vector<calc::Vec3> tangents_;
	std::vector<calc::Vec3> bitangents_;
	std::vector<unsigned> indices_;

	class Part {
	public:
		int vstart;
		int vcount;
		int istart;
		int icount;
		Material material;
	};

	std::vector<Part> parts_;

	AttribCode acode_ = 0;
	static constexpr GLuint primitive_restart_number_ = std::numeric_limits<GLuint>::max();

	std::unique_ptr<Mesh> mesh_;
	ModelType model_type_;
};

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


#endif /* GFX_MODEL_H */
