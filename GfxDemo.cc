// GfxHair.cpp : Defines the entry point for the application.
//

#include <string>
#include <numeric>
#include "utility.h"
#include "GfxModel.h"
#include "GfxDemo.h"
#include "GfxInput.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

int main()
{
	glfwInit();
	auto context = glfwCreateWindow(gfxconfig::winsize.x, gfxconfig::winsize.y, "My Window", NULL, NULL);
	if (!context)
		exit(1);
	glfwMakeContextCurrent(context);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	std::string obj_inputfile{gfxconfig::asset_dir + "\\woman\\woman.obj"};
	auto acode = gfx::vertex_attrib::PosNormUV;
	auto placement = calc::Box3D{{0,0,0},{2,2,2}};

	auto obj = gfx::Model::load_from_obj_file(obj_inputfile, gfx::vertex_attrib::PosNormUV);

	util::print("{}\n", obj.num_parts());
	util::print("#vert={}\n", obj.num_verts());

	//std::string hair_inputfile{"F:\\repo\\GfxDemo\\asset\\woman_straight_hair\\wStraight.ind"};
	//
	//auto hair = gfx::Model::load_from_ind_file(hair_inputfile, gfx::vertex_attrib::PosTan);

	//util::print("#hair groups={}\n", hair.num_parts());

	Renderer renderer{};
	renderer.init();
	renderer.create_framebuffer(gfxconfig::winsize);

	gfx::ArcballCamera camera{obj.bounds(), { 0,0,-1 }, { 0,1,0 },
		calc::to_radian(60.f),
		static_cast<float>(gfxconfig::winsize.x) / gfxconfig::winsize.y};

	gfx::Input input;

	gfx::init_input_with_glfw(context);

	while (!glfwWindowShouldClose(context)) {
		glfwPollEvents();
		gfx::fill_input_with_glfw(context, &input);
		camera.process_input(input);
		auto fbo = renderer.render(obj, camera);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0,
			gfxconfig::winsize.x, gfxconfig::winsize.y,
			0, 0,
			gfxconfig::winsize.x, gfxconfig::winsize.y,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glfwSwapBuffers(context);
	}

	renderer.destory_resource();

	return 0;
}
