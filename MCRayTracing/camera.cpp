#include "camera.h"

Camera::Camera() {
	look_at(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
	calc_view_port(60, 600, 600);
}
void Camera::look_at(Real eye_x, Real eye_y, Real eye_z,
	Real center_x, Real center_y, Real center_z,
	Real up_x, Real up_y, Real up_z) {
	origin = Vec3f(eye_x, eye_y, eye_z);
	direction = Vec3f(center_x - eye_x, center_y - eye_y, center_z - eye_z).normalize();
	up = Vec3f(up_x, up_y, up_z);
	right = cross(direction, up).normalize();
	up = cross(right, direction).normalize();
}
void Camera::calc_view_port(Real _fov, int _width, int _height) {
	fov = _fov;
	width = _width;
	height = _height;
	Real aspect = (Real)height / (Real)width;
	view_x = right * 2 * tan(fov * PI / 360);
	view_y = up * 2 * tan(fov * aspect * PI / 360);
	view_z = direction;
}
Ray Camera::get_ray(Real x, Real y) {
	Vec3f direction = view_z + (x - 0.5f) * view_x + (y - 0.5f) * view_y;
	return Ray(origin, direction);
}

