#include "GfxCamera.h"
#include "GfxConfig.h"
#include "GfxInput.h"

#include "calc.h"

namespace gfx
{

ArcballCamera::ArcballCamera(
    calc::Box3D bounds, 
    calc::Vec3 forward, 
    calc::Vec3 up, 
    float FoVy, 
    float aspect)
{
	bounds_ = bounds;
	forward_ = forward;
	auto right = calc::cross(forward, up);
	up_ = calc::normalize(calc::cross(right, forward));
	FoVy_ = FoVy;
	aspect_ = aspect;

	bounds_radius_ = calc::length(bounds.size()) * .5f;
	dist_ = bounds_radius_ / sin(FoVy * .5f);

	nearp_ = dist_ - bounds_radius_;
	farp_ = dist_ + bounds_radius_;

	init_pos_ = pos();
	init_view_matrix_ = calc::lookat(init_pos_, bounds_.center(), up_);
}

calc::Mat4 ArcballCamera::world_transform() const
{
	auto proj = calc::projective_transform(FoVy_, aspect_, nearp_, farp_);
	calc::Mat4 view;
	if (tracking)
		view = calc::lookat(
            pos(), bounds_.center(), calc::rotate(track_rot_, up_));
	else
		view = calc::lookat(pos(), bounds_.center(), up_);
	return calc::dot(proj, view);
}

calc::Vec3 ArcballCamera::pos() const
{
	if (tracking)
		return bounds_.center() - dist_ * calc::normalize(
            calc::rotate(track_rot_, forward_));
	else
		return bounds_.center() - dist_ * calc::normalize(forward_);
}

void ArcballCamera::process_input(const Input& input)
{
	auto scroll = input.mouse.scroll;
	if (scroll.y > 1e-4)
		FoVy_ = calc::clamp(FoVy_ + .1f, calc::to_radian(10.f), \
            calc::to_radian(120.f));
	if (scroll.y < -1e-4)
		FoVy_ = calc::clamp(FoVy_ - .1f, calc::to_radian(10.f), \
            calc::to_radian(120.f));

	bool press = input.mouse.left_button_press;
	bool hit = false;
	calc::Vec3 nhit{};

	if (press) {
		int w = input.window.width;
		int h = input.window.height;
		auto mpos = input.mouse.cursor;
		float y = h - 1 - mpos.y, x = mpos.x;
		calc::Vec3 wpos{ x,y,.0f };
		calc::Vec4 view_port{ 0, 0,
            static_cast<float>(w), static_cast<float>(h) };

		auto pos = init_pos_;

		auto view_matrix = init_view_matrix_;
		auto proj_matrix = calc::projective_transform(
            FoVy_, aspect_, nearp_, farp_);;
		auto p = calc::unproject(
            wpos, view_matrix, proj_matrix, view_port);
		calc::Sphere sphere{ bounds_.center(), bounds_radius_ };

		calc::Ray ray{ pos,p - pos };

		hit = calc::hit(sphere, ray);
		nhit = (ray.o+ray.s*ray.d) - bounds_.center();
		nhit = calc::normalize(nhit);
	}

	bool tracking_now = press && hit;

	if (!tracking && tracking_now) {
		first_hit_ = nhit;
		track_rot_ = calc::Quat{ 1,0,0,0 };
		tracking = true;
		return;
	}

	if (tracking && !tracking_now) {
		track_rot_prev_ = track_rot_ * track_rot_prev_;
		forward_ = calc::rotate(track_rot_, forward_);
		up_ = calc::rotate(track_rot_, up_);
		if (std::abs(calc::dot(up_, forward_)) > 1e-4) {
			auto right = calc::cross(forward_, up_);
			up_ = calc::cross(right, forward_);
		}
		tracking = false;

		return;
	}

	if (tracking && tracking_now) {
		if (calc::length(nhit - first_hit_) < 1e-4)
			return;

		auto qrot_axis = calc::rotate(
            track_rot_prev_, calc::cross(first_hit_, nhit));

		float dw = -std::acos(
            calc::clamp(calc::dot(first_hit_, nhit), -1.f, 1.f));
		track_rot_ = calc::Quat::angle_axis(
            dw, calc::normalize(qrot_axis));
		return;
	}

}

}
