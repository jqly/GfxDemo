#include "GfxModel.h"

#include <unordered_map>
#include <functional>
#include <cassert>

#include "calc.h"
#include "utility.h"

#ifndef _ANDROID_
#include "glad/glad.h"
#endif

#include "stb_image.h"

namespace gfx
{

template<typename Floats>
void create_array_buffer(
    unsigned attribid, GLuint& buffer, 
    const std::vector<Floats>& data)
{
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(Floats) * data.size(), 
        data.data(), 
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(attribid);
    glVertexAttribPointer(
        attribid, sizeof(Floats)/sizeof(float), GL_FLOAT, 
        GL_FALSE, sizeof(Floats), (void*)(0));
}

void create_element_array_buffer(GLuint& buffer,
    const std::vector<GLuint>& indices)
{
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        indices.size() * sizeof(indices[0]), 
        indices.data(), GL_STATIC_DRAW);
}

Mesh::Mesh(const Model& model)
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    if (model.acode_ == vertex_attrib::PosNormUV) {
        create_array_buffer(0, pos_, model.positions_);
        create_array_buffer(1, norm_, model.normals_);
        create_array_buffer(2, uv_, model.uvs_);
    } else if (model.acode_ == vertex_attrib::PosTan) {
        create_array_buffer(0, pos_, model.positions_);
        create_array_buffer(1, tan_, model.tangents_);
    } else {
        std::cerr << "Mesh type not supported.\n";
        exit(1);
    }

    if (!model.indices_.empty())
        create_element_array_buffer(ebo_, model.indices_);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &pos_);
    glDeleteBuffers(1, &norm_);
    glDeleteBuffers(1, &uv_);
    glDeleteBuffers(1, &tan_);    
    glDeleteBuffers(1, &bitan_);
    glDeleteBuffers(1, &ebo_);
}

class TinyobjModel {
public:
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
};

void fit_model_placement(float* positions, int size, 
    calc::Box3D placement)
{
    calc::Box3D geom{};
    for (int v = 0; v < size/3; ++v) {
        calc::Vec3 pos;
        std::copy_n(&positions[3*v], 3, 
            calc::begin(pos));
        geom.update(pos);
    }
    auto center = placement.center();
    auto scale_ = placement.size() / geom.size();
    auto scale = *std::min_element(
        calc::begin(scale_), calc::end(scale_));
    for (int v = 0; v < size/3; ++v) {
        calc::Vec3 pos;
        std::copy_n(&positions[3*v], 3, 
            calc::begin(pos));
        pos = (pos-geom.center())*scale + center;
        std::copy_n(calc::begin(pos), 3,
        &positions[3*v]);
    }
}

TinyobjModel load_tinyobj_model(const std::string& objfile, 
    const std::string& mtldir,
    calc::Box3D placement = {{0,0,0},{-1,-1,-1}})
{
    TinyobjModel model;
    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(
        &model.attrib, &model.shapes, &model.materials, 
        &warn, &err, objfile.c_str(), mtldir.c_str(), true);

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    if (placement.size().x > 0) {
        fit_model_placement(model.attrib.vertices.data(), 
            model.attrib.vertices.size(),placement);
    }

	return model;
}

struct ModelVertex {
    calc::Vec3 position;
    calc::Vec3 normal;
    calc::Vec2 texcoord;
    calc::Vec3 tangent;
    calc::Vec3 bitangent;
    unsigned fence;
};

bool operator==(const ModelVertex& lhs, const ModelVertex& rhs)
{
    return (lhs.position == rhs.position) 
        && (lhs.normal == rhs.normal)
        && (lhs.tangent == rhs.tangent)
        && (lhs.bitangent == rhs.bitangent)
        && (lhs.texcoord == rhs.texcoord)
        && (lhs.fence == rhs.fence);
}

}

template <>
struct std::hash<gfx::ModelVertex> {
    std::size_t operator()(const gfx::ModelVertex& vert) const
    {
		auto unsigned_hasher = std::hash<unsigned>();
        std::size_t seed = 0xdeadbeefc01dbeaf;
        seed = calc::hash_combine(seed, calc::hash(vert.position));
        seed = calc::hash_combine(seed, calc::hash(vert.normal));
        seed = calc::hash_combine(seed, calc::hash(vert.tangent));
        seed = calc::hash_combine(seed, calc::hash(vert.bitangent));
        seed = calc::hash_combine(seed, calc::hash(vert.texcoord));
		seed = calc::hash_combine(seed, unsigned_hasher(vert.fence));

        return seed;
    }
};

namespace gfx 
{

// TODO: Compute tangent & bitangent.
ModelVertex get_ModelVertex(AttribCode acode, 
    const tinyobj::index_t& idx,
    const tinyobj::attrib_t& attrib)
{
    ModelVertex vert{};
    if (acode & vertex_attrib::Pos) {
        std::copy_n(
            &attrib.vertices[3*idx.vertex_index],
            3, calc::begin(vert.position));
    }
    if (acode & vertex_attrib::Norm) {
        std::copy_n(
            &attrib.normals[3*idx.normal_index],
            3, calc::begin(vert.normal));
    }
    if (acode & vertex_attrib::UV) {
        std::copy_n(
            &attrib.texcoords[2*idx.texcoord_index],
            2, calc::begin(vert.texcoord));
    }
	return vert;
}

Model Model::load_from_obj_file(const std::string& objfile, 
    AttribCode acode,
	calc::Box3D placement)
{
	auto mtldir = util::get_file_base_dir(objfile);
    auto obj = load_tinyobj_model(objfile, mtldir, placement);

    Model model{};
    model.acode_ = acode;
    model.model_type_ = ModelType::TriangleMesh;   

    ////
    // Each parts relates to two set of arrays.
    // One is the vertex array and the other is 
    // the index array. Each `shape` posesses 
    // contiguous storage inside the above two 
    // seperatly. And we record the start and 
    // size to keep track of these information.
    // To avoid duplications, we use 
    // `unordered_map.` We introduce the fence 
    // to seperate two identical vertices in 
    // the two parts.
    ////
	
    std::unordered_map<ModelVertex, unsigned> vimap;
    unsigned vcount = 0, icount = 0;
    int fence = 0;

    model.parts_.resize(obj.shapes.size());

	for (size_t s = 0; s < obj.shapes.size(); s++) {

		if (s != 0) {
            auto& prev_part = model.parts_[s-1];
            prev_part.vcount = vcount - prev_part.vstart;
            prev_part.icount = icount - prev_part.istart;
            fence++;
        }
        auto& part = model.parts_[s];
        part.vstart = vcount;
        part.istart = icount;
		size_t fvstart = 0;

		auto& mesh = obj.shapes[s].mesh;

		if (!mesh.material_ids.empty()) {
			auto material_id = mesh.material_ids[0];

			for (auto id: mesh.material_ids)
				if (id != material_id) {
					std::clog << "obj shapes not grouped by material.\n";
					break;
				}

			const auto& smtl = obj.materials[material_id];
            auto& tmtl = part.material;
			std::copy_n(smtl.ambient, 3, calc::begin(tmtl.ambient));
			std::copy_n(smtl.diffuse, 3, calc::begin(tmtl.diffuse));
			std::copy_n(smtl.specular, 3, calc::begin(tmtl.specular));

#ifndef _WIN32
			const char dirsep = '/';
#else
			const char dirsep = '\\';
#endif

			if (!smtl.ambient_texname.empty())
				tmtl.ambient_texpath = mtldir + dirsep + \
                    smtl.ambient_texname;
			if (!smtl.diffuse_texname.empty())
				tmtl.diffuse_texpath = mtldir + dirsep + \
                    smtl.diffuse_texname;
			if (!smtl.specular_texname.empty())
				tmtl.specular_texpath = mtldir + dirsep + \
                    smtl.specular_texname;

			//util::print("ambient {},{}\n", tmtl.ambient,tmtl.ambient_texpath);
			//util::print("diffuse {},{}\n", tmtl.diffuse,tmtl.diffuse_texpath);
			//util::print("specular {},{}\n", tmtl.specular,tmtl.specular_texpath);

		}

		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {			

			int fvcount = mesh.num_face_vertices[f];
            for (int fv = 0; fv < fvcount; ++fv) {
                auto idx = mesh.indices[fvstart+fv];
				
                auto vert = get_ModelVertex(acode, idx, obj.attrib);
                vert.fence = fence;
                auto probe = vimap.find(vert);
                if (probe != vimap.end())
                    model.indices_.push_back(probe->second);
                else {
                    model.positions_.push_back(vert.position);
                    if (acode & vertex_attrib::Norm)
                        model.normals_.push_back(vert.normal);
                    if (acode & vertex_attrib::UV)
                        model.uvs_.push_back(vert.texcoord);
					vimap.insert({vert,vcount});
                    model.indices_.push_back(vcount);
                    // TODO: Compute tangent & bitangent.
                    vcount++;
                }
                icount++;
            }

			fvstart += fvcount;
		}
	}

	assert(!model.parts_.empty());

    auto& last_part = model.parts_.back();
    last_part.vcount = vcount - last_part.vstart;
    last_part.icount = icount - last_part.istart;

    model.bounds_ = calc::box_from_points(model.positions_);

    return model;
}

template<typename T>
T ifstream_read(std::ifstream& fp)
{
	T val;
	fp.read(reinterpret_cast<char*>(&val), sizeof(val));
	return val;
}

Model Model::load_from_ind_file(
    const std::string& inputfile, 
    AttribCode acode,
	calc::Box3D placement)
{
    std::ifstream fp(inputfile, std::ios::binary);
    char header[9];
    fp.read(header, 8);
    header[8] = '\0';
    if (strcmp(header, "IND_HAIR") != 0) {
        std::cerr << "Wrong input.\n";
        exit(1);
    }
    auto num_fibers = ifstream_read<unsigned>(fp);
    auto num_verts = ifstream_read<unsigned>(fp);

	//util::print("#hair fibers = {}\n", num_fibers);

    Model model{};
    model.acode_ = acode;
    model.model_type_ = ModelType::Hair;    

    // Currently, our hair model has one part,
    // with primitive restart number seperating 
    // the fibers.
    model.positions_.reserve(num_verts);
    model.indices_.reserve(num_fibers-1 + num_verts);

    auto vcnt = 0;
    for (int p = 0; p < num_fibers; ++p) {
        auto num_pverts = ifstream_read<unsigned>(fp);

        if (num_pverts == 0)
            continue;
        if (num_pverts == 1) {
            ifstream_read<calc::Vec3>(fp);
            continue;
        }

        for (int pv = 0; pv < num_pverts; ++pv) {
            auto position = ifstream_read<calc::Vec3>(fp);
            model.positions_.push_back(position);
            model.indices_.push_back(vcnt + pv);
        }

        if (p != num_fibers - 1)
            model.indices_.push_back(Model::primitive_restart_number_);

        vcnt += num_pverts;
    }

	assert(model.positions_.size() == num_verts);
    assert(model.indices_.size() == num_fibers-1+num_verts);

    model.parts_.resize(1);
    model.parts_[0].vstart = 0;
    model.parts_[0].vcount = num_verts;
    model.parts_[0].istart = 0;
    model.parts_[0].icount = num_fibers-1+num_verts;

    if (placement.size().x > 0) {
        fit_model_placement((float*)model.positions_.data(), 
            model.positions_.size()*3, placement);
    }

    model.bounds_ = calc::box_from_points(model.positions_);

    if (acode&vertex_attrib::Tan == 0)
        return model;

    model.tangents_.resize(num_verts);
    for (int p = 0; p < model.parts_.size(); ++p) {
        auto start = model.parts_[p].vstart;
        auto pvcnt = model.parts_[p].vcount;

        model.tangents_[start] = calc::normalize(
            model.positions_[start+1]-model.positions_[start]);

        for (int v = 1; v < pvcnt-1; ++v) {
            auto forward_tangent = calc::normalize(
                model.positions_[start+v+1]-model.positions_[start+v]);
            model.tangents_[start+v] = calc::normalize(
                model.tangents_[start+v-1]+forward_tangent);
        }

        model.tangents_[start+pvcnt-1] = calc::normalize(
            model.positions_[start+pvcnt-1] - 
            model.positions_[start+pvcnt-2]);

    }
    assert(model.tangents_.size() == num_verts);

    return model;
}

ModelType Model::model_type() const { return model_type_; }

void Model::init_mesh()
{
    if (!mesh_)
        mesh_ = std::make_unique<Mesh>(*this);
}

calc::Mat4 Model::local_transform() const
{
    return model_matrix_;
}

void Model::local_transform(calc::Mat4 matrix)
{
    model_matrix_ = matrix;

    std::vector<calc::Vec3> corners{
        { 1, 1, 1},
        { 1, 1,-1},
        { 1,-1, 1},
        { 1,-1,-1},
        {-1, 1, 1},
        {-1, 1,-1},
        {-1,-1, 1},
        {-1,-1,-1}};

    for (auto& corner : corners) {
        corner = corner*bounds_.size()*.5f + bounds_.center();
        corner = calc::point_transform(model_matrix_, corner);
    }

    bounds_ = calc::box_from_points(corners);
}

void Model::bind_mesh()
{
    init_mesh();
    assert(mesh_->vao());
    glBindVertexArray(mesh_->vao());
    if (acode_ == vertex_attrib::PosNormUV) {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    } else if (acode_ == vertex_attrib::PosTan) {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    } else {
        std::cerr << "Mesh not supported.\n";
        exit(1);				
    }

    if (model_type_ == ModelType::Hair) {
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(primitive_restart_number_);
    }
}

void Model::unbind_mesh()
{
    assert(mesh_->vao());
    glBindVertexArray(mesh_->vao());
    if (acode_ == vertex_attrib::PosNormUV) {
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    } else if (acode_ == vertex_attrib::PosTan) {
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    } else {
        std::cerr << "Mesh not supported.\n";
        exit(1);				
    }

    if (model_type_ == ModelType::Hair)
        glDisable(GL_PRIMITIVE_RESTART);
}

int Model::num_verts() const
{
	return positions_.size();
}

int Model::num_parts() const
{
    return parts_.size();
}

const Material& Model::material(int part_idx) const
{
    return parts_[part_idx].material;
}

void Model::draw(int part_idx) const
{
    assert(!indices_.empty());

    switch (model_type_) {
	case ModelType::TriangleMesh:
        glDrawElements(GL_TRIANGLES, parts_[part_idx].icount, 
            GL_UNSIGNED_INT, (GLvoid*)parts_[part_idx].istart);
        break;
    case ModelType::Hair:
        glDrawElements(GL_LINE_STRIP, parts_[part_idx].icount, 
            GL_UNSIGNED_INT, (GLvoid*)parts_[part_idx].istart);
        break;
    default:
        break;
    }
}

calc::Box3D Model::bounds() const 
{
    return bounds_;
}

}

