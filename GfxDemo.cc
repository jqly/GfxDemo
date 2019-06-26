// GfxHair.cpp : Defines the entry point for the application.
//

#include <string>
#include <numeric>
#include "utility.h"
#include "GfxModel.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

int main()
{
	glfwInit();
	auto context = glfwCreateWindow(640, 480, "My Window", NULL, NULL);
	if (!context)
		exit(1);
	glfwMakeContextCurrent(context);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	std::string obj_inputfile{"F:\\repo\\GfxDemo\\asset\\woman\\woman.obj"};
	auto acode = gfx::vertex_attrib::PosNormUV;
	auto placement = calc::Box3D{{0,0,0},{2,2,2}};

	auto obj = gfx::Model::load_from_obj_file(obj_inputfile, gfx::vertex_attrib::PosNormUV);

	util::print("{}\n", obj.num_parts());
	util::print("#vert={}\n", obj.num_verts());

	std::string hair_inputfile{"F:\\repo\\GfxDemo\\asset\\woman_straight_hair\\wStraight.ind"};
	
	auto hair = gfx::Model::load_from_ind_file(hair_inputfile, gfx::vertex_attrib::PosTan);

	util::print("#hair groups={}\n", hair.num_parts());

	return 0;
}
