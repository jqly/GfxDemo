
#ifndef GFX_DEMO_H
#define GFX_DEMO_H


#include <iostream>
#include "glad/glad.h"
#include "GfxCamera.h"
#include "GfxModel.h"
#include "GfxShader.h"
#include "calc.h"
#include "GfxConfig.h"

void fbo_with_color_rgba8_texture_depth_24_renderbuffer(
    calc::iVec2 size, GLuint* fbo, GLuint* color, GLuint* depth)
{

    glGenTextures(1, color);
    glBindTexture(GL_TEXTURE_2D, *color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, size.x, size.y);

	glGenRenderbuffers(1, depth);
	glBindRenderbuffer(GL_RENDERBUFFER, *depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *color, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *depth);  

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "framebuffer incomplete.\n";
        exit(1);
    }
}

class Renderer : public gfx::Shader {
public:

    Renderer()
    {}

    void init()
    {
        program_ = gfx::create_glsl_program(
            std::unordered_map<std::string, std::string>{
                {"version", "#version 450 core"}
            },
            gfxconfig::shader_dir + "\\mesh_with_texture.glsl");
    }

    void create_framebuffer(calc::iVec2 rtsize)
    {
        fbo_with_color_rgba8_texture_depth_24_renderbuffer(
            rtsize, &fbo_, &color_, &depth_);
        rtsize_ = rtsize;
    }

	GLuint render(gfx::Model& model, gfx::Camera& camera)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, rtsize_.x, rtsize_.y);

        glEnable(GL_DEPTH_TEST);

		glClearColor(1,1,1,1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(program_);

        set_uniform(program_, "g_DiffuseMap", 0);

        auto local_transform = model.local_transform();
        auto world_transform = camera.world_transform();
        set_uniform(program_, "g_WorldTransform", world_transform);
        set_uniform(program_, "g_Eye", camera.pos());
        set_uniform(program_, "g_PointLightPos", gfxconfig::point_light_pos);
        set_uniform(program_, "g_LocalTransform", local_transform);

		int num_parts = model.num_parts();

		model.bind_mesh();
		for (int p = 0; p < num_parts; ++p) {
			auto& material = model.material(p);
			glActiveTexture(GL_TEXTURE0);
			gfx::TexInfo info{material.diffuse_texpath, GL_LINEAR,GL_LINEAR,1};
			GLuint tex = load_texture(info);
			glBindTexture(GL_TEXTURE_2D, tex);
			model.draw(p);
		}
		model.unbind_mesh();
		return fbo_;
    }

    void destory_resource()
    {
        glDeleteProgram(program_);
        glDeleteFramebuffers(1, &fbo_);
        glDeleteTextures(1, &color_);
        glDeleteTextures(1, &depth_);
    }

private:
    GLuint program_ = 0;

    // Render target
    calc::iVec2 rtsize_;
    GLuint fbo_ = 0, color_ = 0, depth_ = 0;
};


#endif
