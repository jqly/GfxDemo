#include "calc.h"
#include "GfxInput.h"

namespace gfx
{

class Camera {
public:
	virtual calc::Mat4 world_transform() const = 0;
	virtual calc::Vec3 pos() const = 0;
};

class ArcballCamera : public Camera {
public:
	ArcballCamera(
		calc::Box3D bounds, 
		calc::Vec3 forward, 
		calc::Vec3 up, 
		float FoVy, 
		float aspect);

	calc::Mat4 world_transform() const override;
	calc::Vec3 pos() const override;

	void process_input(const Input& input);

private:
	calc::Box3D bounds_;
	calc::Vec3 forward_, up_;
	float FoVy_, aspect_, bounds_radius_, dist_, nearp_, farp_;
	bool tracking{ false };
	calc::Quat track_rot_{ 1,0,0,0 }, track_rot_prev_{ 1,0,0,0 };
	calc::Vec3 first_hit_;
	calc::Mat4 init_view_matrix_;
	calc::Vec3 init_pos_;
};

}

