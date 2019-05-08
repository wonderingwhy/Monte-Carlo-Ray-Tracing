#include "common.h"
bool is_equal(Real a, Real b) { return fabs(a - b) < EPS; }
Real sqr(Real x) { return x * x; }
Vec3f Vec3f::normalize() { return *this / sqrt(dot(*this, *this)); }
Real Vec3f::length() { return sqrt(sqr(this->x) + sqr(this->y) + sqr(this->z)); }
Real dot(const Vec3f &u, const Vec3f &v) { return u.x * v.x + u.y * v.y + u.z * v.z; }
Vec3f cross(const Vec3f &u, const Vec3f &v) {
	return Vec3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}