
#ifndef GFX_MODEL_H
#define GFX_MODEL_H

#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "calc.h"
#include "tinyobjloader/tiny_obj_loader.h"
#ifndef _ANDROID_
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#endif

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
	GLuint vao() const { return vao_; }

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

	ModelType model_type() const;

	calc::Mat4 local_transform() const;
	void local_transform(calc::Mat4 matrix);
	// Bind/Unbind vao and enable/disable attribs
	void init_mesh();
	void bind_mesh();
	void unbind_mesh();

	int num_verts() const;
	int num_parts() const;
	const Material& material(int part_idx) const;
	void draw(int part_idx) const;

	calc::Box3D bounds() const;

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
	static constexpr GLuint primitive_restart_number_ = \
		std::numeric_limits<GLuint>::max();

	std::unique_ptr<Mesh> mesh_;
	ModelType model_type_;

	calc::Box3D bounds_;
	calc::Mat4 model_matrix_ = calc::diag<calc::Mat4>(1.f);
};

}



#endif /* GFX_MODEL_H */
