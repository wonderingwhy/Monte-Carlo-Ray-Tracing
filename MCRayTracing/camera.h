#pragma once

#include "ray.h"

class Camera {
public:
	int width, height;
	Vec3f origin;
	Vec3f direction;
	Real fov;
	Vec3f front, up, right;
	Vec3f view_x, view_y, view_z;
	Camera();
	void look_at(Real eye_x, Real eye_y, Real eye_z,
		Real center_x, Real center_y, Real center_z,
		Real up_x, Real up_y, Real up_z);
	void calc_view_port(Real _fov, int _width, int _height);
	Ray get_ray(Real x, Real y);
};
