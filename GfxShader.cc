#include <sstream>
#include <cassert>
#include "GfxShader.h"
#include "utility.h"
#include "stb_image.h"

namespace gfx
{

TexInfo::TexInfo(
    std::string path, 
    GLint mag_filter, 
    GLint min_filter, 
    GLsizei num_mipmaps)
    : path{path}, 
    mag_filter{mag_filter}, 
    min_filter{min_filter},
    num_mipmaps{num_mipmaps}
{}

bool operator==(const TexInfo& lhs, const TexInfo& rhs)
{
    return (lhs.path == rhs.path) 
        && (lhs.mag_filter == rhs.mag_filter)
        && (lhs.min_filter == rhs.min_filter)
        && (lhs.num_mipmaps == rhs.num_mipmaps);
}

GLuint create_texture_from_memory(
    const std::string& img, 
    const TexInfo& info)
{
	int w, h, c;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load_from_memory(
		(unsigned char*)img.data(), img.size(), &w, &h, &c, 0);
	
	if (data == nullptr) {
		std::cerr << "Failed to load image.\n";
		exit(1);
	}

	GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

	if (c == 4) {
        glTexStorage2D(GL_TEXTURE_2D, info.num_mipmaps, GL_RGBA8, w, h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, \
            GL_RGBA, GL_UNSIGNED_BYTE, data);
	} else if (c == 3) {
        glTexStorage2D(GL_TEXTURE_2D, info.num_mipmaps, GL_RGB8, w, h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, \
            GL_RGB, GL_UNSIGNED_BYTE, data);
	} else if (c == 1) {
        glTexStorage2D(GL_TEXTURE_2D, info.num_mipmaps, GL_R8, w, h);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, \
            GL_RED, GL_UNSIGNED_BYTE, data);
	} else {
		std::cerr << "Unsupported texture format(#channels not 1, 3 or 4)";
		exit(1);
	}

    if (info.num_mipmaps > 1)
        glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, info.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info.min_filter);

	stbi_image_free(data);
	return tex;
}

GLuint TextureLoader::load(const TexInfo& info)
{
	assert(std::find(fmts_.begin(),fmts_.end(),
		util::get_file_extension(info.path)) != fmts_.end());

	auto img_found = rawimgs_.find(info.path);

	if (img_found == rawimgs_.end()) {
		auto img = util::read_file(info.path);
		rawimgs_.insert({info.path, img});
		auto tex = create_texture_from_memory(img, info);
		texs_.insert({info, tex});
		return tex;
	}

    auto tex_found = texs_.find(info);

    assert(tex_found != texs_.end());

    //GLboolean stats;
    //glAreTexturesResident(1, &tex_found->second, &stats);
    //if (stats == 0) {
    //    glDeleteTextures(1, &tex_found->second);
    //    auto tex = create_texture_from_memory(
    //        img_found->second, info);
    //    tex_found->second = tex;
    //}
    return tex_found->second;
}

GLuint UniformSetter::get_uniform(GLuint program, const std::string& name)
{
    auto key = std::to_string(program) + name;
    auto found = uniforms_.find(key);
    if (found == uniforms_.end()) {
        auto uniform = glGetUniformLocation(program, name.c_str());
        uniforms_.insert({key,uniform});
        return uniform;
    }
    return found->second;
}

template <>
void UniformSetter::template set(GLuint program, const std::string& name, const calc::Vec2& value)
{
    glUniform2f(get_uniform(program, name), value.x, value.y);
}

template <>
void UniformSetter::template set(GLuint program, const std::string& name, const calc::Vec3& value)
{
    glUniform3f(get_uniform(program, name), value.x, value.y, value.z);
}

template <>
void UniformSetter::template set(GLuint program, const std::string& name, const calc::Vec4& value)
{
    glUniform4f(get_uniform(program, name), value.x, value.y, value.z, value.w);
}

template <>
void UniformSetter::template set(GLuint program, const std::string& name, const calc::Mat4& value)
{
    glUniformMatrix4fv(get_uniform(program, name), 1, GL_FALSE, calc::begin(value));
}

template <>
void UniformSetter::template set(GLuint program, const std::string& name, const GLint& value)
{
    glUniform1i(get_uniform(program, name), value);
}

template <> 
void UniformSetter::template set(GLuint program, const std::string& name, const GLfloat& value)
{
    glUniform1f(get_uniform(program, name), value);
}

template <>
void UniformSetter::template set(GLuint program, const std::string& name, const GLuint& value)
{
    glUniform1ui(get_uniform(program, name), value);
}

std::string prettify_shader_log(const std::string & log, const std::string & shader)
{
	std::ostringstream ss;

	std::vector<int> line_begin_locations;

	for (int i = 0; i < shader.size(); ++i) {
		auto ch = shader[i];

		if (i == 0)
			line_begin_locations.push_back(0);
		else if (ch == '\n' && i + 1 < shader.size())
			line_begin_locations.push_back(i + 1);
		else
			;
	}

	int state = 0, log_line_begin = 0;
	std::string which_line;

	for (int idx = 0; idx < log.size(); ++idx) {

		char next_ch = log[idx];

		switch (state) {


		case 0:
			if (std::isdigit(next_ch)) {
				log_line_begin = idx;
				state = 1;
			}
			break;

		case 1:
			if (next_ch == '(')
				state = 2;
			else if (!std::isdigit(next_ch))
				state = 0;
			break;

		case 2:
			if (std::isdigit(next_ch)) {
				which_line.clear();
				which_line += next_ch;
				state = 3;
			}

			else
				state = 0;
			break;

		case 3:

			if (next_ch == ')' && idx + 2 < log.size() && log[idx + 1] == ' ' && log[idx + 2] == ':')
				state = 4;
			else if (std::isdigit(next_ch))
				which_line += next_ch;
			else
				state = 0;
			break;

		case 4:
		{
			int lino = std::stoi(which_line);
			int off = line_begin_locations[std::max(0, lino - 2)];
			int total_lines = line_begin_locations.size();
			int count = 0;
			if (total_lines <= lino + 1)
				count = shader.size() - off;
			else
				count = line_begin_locations[lino + 1] - off;
			util::print(ss, "----------\n");
			while (count > 0 && shader[off + count - 1] == '\n')
				count--;
			util::print(ss, "{}\n", shader.substr(off, count));

			int skip = 0;
			while (skip + idx < log.size() && log[idx + skip] != '\n')
				++skip;
			util::print(ss, "{}\n", log.substr(log_line_begin, idx - log_line_begin + skip));
			util::print(ss, "----------\n");
			state = 0;
		}
		break;

		default:
			break;
		}
	}

	return ss.str();
}

GLuint create_shader(GLenum type, std::string code)
{
	GLuint shader_handle = glCreateShader(type);
	auto code_ = code.c_str();
	glShaderSource(shader_handle, 1, &code_, nullptr);
	glCompileShader(shader_handle);

	int success = 0, log_len = 0;
	glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);

	if (!success) {
		std::string shader_type_name{ "Unknown shader" };
		switch (type) {
		case GL_VERTEX_SHADER:
			shader_type_name = "Vertex shader";
			break;
		case GL_FRAGMENT_SHADER:
			shader_type_name = "Fragment shader";
			break;
		case GL_GEOMETRY_SHADER:
			shader_type_name = "Geometry shader";
			break;
		case GL_COMPUTE_SHADER:
			shader_type_name = "Compute shader";
			break;
		default:
			std::cerr << "Unknown type.\n";
			exit(1);
		}

		glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len <= 0)
			util::print(std::cerr, "{}: no log found", shader_type_name);
		else {
			auto log = std::unique_ptr<char>(new char[log_len]);
			glGetShaderInfoLog(shader_handle, log_len, &log_len, log.get());
			util::print(std::cerr, "======={} compile log=======\n", shader_type_name);
			auto pretty_log = prettify_shader_log(std::string(log.get(), log_len), code);
			util::print(std::cerr, "{}\n", pretty_log);
			util::print(std::cerr, "-------\n");
		}
		util::print(std::cerr, "{} compile error.\n", shader_type_name);
        exit(1);
	}
	return shader_handle;
}

void link_shader(GLuint program)
{
    glLinkProgram(program);
	int success = 0, log_len = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len <= 0)
			util::print(std::cerr, "No log found\n");
		else {
			auto log = std::unique_ptr<char>(new char[log_len]);
			glGetProgramInfoLog(program, log_len, nullptr, log.get());
			util::print(std::cerr, "Program compile log: {}", log.get());
		}
		util::print(std::cerr, "Program compile error");
        exit(1);
	}
}

GLuint create_program(std::string comp_shader)
{
	GLuint compobj = create_shader(GL_COMPUTE_SHADER, comp_shader);

	GLuint handle = glCreateProgram();
	glAttachShader(handle, compobj);
	glLinkProgram(handle);

	link_shader(handle);

	glDeleteShader(compobj);

	return handle;
}

GLuint create_program(std::string vert_shader, std::string frag_shader)
{
	auto vertobj = create_shader(GL_VERTEX_SHADER, vert_shader);
	auto fragobj = create_shader(GL_FRAGMENT_SHADER, frag_shader);

	GLuint handle = glCreateProgram();
	glAttachShader(handle, vertobj);
	glAttachShader(handle, fragobj);
	glLinkProgram(handle);

	link_shader(handle);

	glDeleteShader(vertobj);
	glDeleteShader(fragobj);

	return handle;
}

GLuint create_program(std::string vert_shader, std::string geom_shader, std::string frag_shader)
{
	auto vertobj = create_shader(GL_VERTEX_SHADER, vert_shader);
	auto geomobj = create_shader(GL_GEOMETRY_SHADER, geom_shader);
	auto fragobj = create_shader(GL_FRAGMENT_SHADER, frag_shader);

	GLuint handle = glCreateProgram();
	glAttachShader(handle, vertobj);
	glAttachShader(handle, geomobj);
	glAttachShader(handle, fragobj);
	glLinkProgram(handle);

	link_shader(handle);

	glDeleteShader(vertobj);
	glDeleteShader(geomobj);
	glDeleteShader(fragobj);

	return handle;
}

GLuint create_glsl_program(
    const std::unordered_map<std::string, std::string> & symbols, 
    const std::string& filepath)
{
	std::string vertex_shader, geometry_shader, fragment_shader, compute_shader;
    auto shader_include_dir = util::get_file_base_dir(filepath);
	auto glsl = std::istringstream(util::read_file(filepath));
	std::string line;

	std::string* stage = nullptr;

	while (std::getline(glsl, line)) {
		if (line.find("#include") != std::string::npos && stage != nullptr) {
			int i = 0, j = 0;
			while (i < line.size() && line[i++] != '\"')
				;
			j = i;
			while (++j < line.size() && line[j] != '\"')
				;

			auto symbol = line.substr(i, j - i);
			if (symbols.count(symbol))
				* stage += symbols.at(symbol);
			else
				*stage += util::read_file(shader_include_dir + line.substr(i, j - i));
			*stage += "\n";
		}
		else if (line.find("#stage vertex") != std::string::npos) {
			stage = &vertex_shader;
		}
		else if (line.find("#stage geometry") != std::string::npos) {
			stage = &geometry_shader;
		}
		else if (line.find("#stage fragment") != std::string::npos) {
			stage = &fragment_shader;
		}
		else if (line.find("#stage compute") != std::string::npos) {
			stage = &compute_shader;
		}
		else if (line.find("#endstage") != std::string::npos) {
			stage = nullptr;
		}
		else {
			if (stage != nullptr)
				* stage += line + "\n";
		}

	}

	int status = 0;
	if (!vertex_shader.empty())
		status |= 1;
	if (!geometry_shader.empty())
		status |= 2;
	if (!fragment_shader.empty())
		status |= 4;
	if (!compute_shader.empty())
		status |= 8;

	GLuint shader = 0;
	if (status == 5)
		shader = create_program(vertex_shader, fragment_shader);
	else if (status == 7)
		shader = create_program(vertex_shader, geometry_shader, fragment_shader);
	else if (status == 8)
		shader = create_program(compute_shader);
	else
		;

	return shader;
}

UniformSetter Shader::uniform_setter_{};
TextureLoader Shader::texture_loader_{};

GLuint Shader::load_texture(const TexInfo& info)
{
    return texture_loader_.load(info);
}

}
