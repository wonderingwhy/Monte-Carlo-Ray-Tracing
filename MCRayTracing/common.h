#pragma once
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
typedef float Real;

const Real EPS = 1e-4f;
const Real PI = acos(-1.0);
const Real INF = FLT_MAX;

using namespace std;

bool is_equal(Real a, Real b);
Real sqr(Real x);
class Vec3f {
public:
	union {
		struct { Real x, y, z; };
		struct { Real r, g, b; };
		Real num[3];
	};

	Vec3f(Real _x = 0.0, Real _y = 0.0, Real _z = 0.0): x(_x), y(_y), z(_z) {};

	Vec3f operator - () { return Vec3f(-x, -y, -z); }
	Vec3f& operator = (const Vec3f &v) { x = v.x; y = v.y; z = v.z; return *this; }
	Vec3f& operator += (Real num) { x += num; y += num; z += num; return *this; }
	Vec3f& operator += (const Vec3f &v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vec3f& operator -= (Real num) { x -= num; y -= num; z -= num; return *this; }
	Vec3f& operator -= (const Vec3f &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vec3f& operator *= (Real num) { x *= num; y *= num; z *= num; return *this; }
	Vec3f& operator *= (const Vec3f &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	Vec3f& operator /= (Real num) { x /= num; y /= num; z /= num; return *this; }
	Vec3f& operator /= (const Vec3f &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }


	friend Vec3f operator + (const Vec3f &u, Real num) { return Vec3f(u.x + num, u.y + num, u.z + num); }
	friend Vec3f operator + (Real num, const Vec3f &u) { return Vec3f(num + u.x, num + u.y, num + u.z); }
	friend Vec3f operator + (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x + v.x, u.y + v.y, u.z + v.z); }
	friend Vec3f operator - (const Vec3f &u, Real num) { return Vec3f(u.x - num, u.y - num, u.z - num); }
	friend Vec3f operator - (Real num, const Vec3f &u) { return Vec3f(num - u.x, num - u.y, num - u.z); }
	friend Vec3f operator - (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x - v.x, u.y - v.y, u.z - v.z); }
	friend Vec3f operator * (const Vec3f &u, Real num) { return Vec3f(u.x * num, u.y * num, u.z * num); }
	friend Vec3f operator * (Real num, const Vec3f &u) { return Vec3f(num * u.x, num * u.y, num * u.z); }
	friend Vec3f operator * (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x * v.x, u.y * v.y, u.z * v.z); }
	friend Vec3f operator / (const Vec3f &u, Real num) { return Vec3f(u.x / num, u.y / num, u.z / num); }
	friend Vec3f operator / (Real num, const Vec3f &u) { return Vec3f(num / u.x, num / u.y, num / u.z); }
	friend Vec3f operator / (const Vec3f &u, const Vec3f &v) { return Vec3f(u.x / v.x, u.y / v.y, u.z / v.z); }

	bool operator == (const Vec3f &u) { return is_equal(x, u.x) && is_equal(y, u.y) && is_equal(z, u.z); }
	bool operator != (const Vec3f &u) { return !(is_equal(x, u.x) && is_equal(y, u.y) && is_equal(z, u.z)); }

	Vec3f normalize();
	Real length();
};
Real dot(const Vec3f &u, const Vec3f &v);
Vec3f cross(const Vec3f &u, const Vec3f &v);
class Vec3i {
public:
	union {
		struct { int x, y, z; };
		struct { int r, g, b; };
		int num[3];
	};
	Vec3i(int _x = 0, int _y = 0, int _z = 0) :x(_x), y(_y), z(_z) {}
	Vec3i(int _num[3]) { memcpy(this, _num, sizeof(this)); }
};
