#pragma once

#include "common.h"
enum SOURCE { NONE, DIFFUSE, SPECULAR, REFRACTION};

class Ray {
public:
	Vec3f origin;
	Vec3f direction;
	SOURCE source;
	Real r_min, r_max;

	Ray(Vec3f _origin = Vec3f(0.0, 0.0, 0.0),
		Vec3f _direction = Vec3f(0.0, 0.0, 0.0), 
		SOURCE _source = SOURCE::NONE) {
		origin = _origin;
		direction = _direction.normalize();
		source = _source;
		r_min = EPS;
		r_max = INF;
	}

	Vec3f reach_point(Real t) const { return origin + direction * t; }
	bool is_in(Real r) const { return r >= r_min && r <= r_max; }
	Vec3f reflect(const Vec3f& normal) { return direction - 2.0 * dot(normal, direction) * normal; }
	bool refract(const Vec3f& normal, Real ni, Vec3f& refract_direction) {
		Real nxd = dot(normal, direction);
		Real k = 1.0 - sqr(ni) * (1.0 - sqr(nxd));
		if (k < 0.0) {
			return false;
		}
		refract_direction = ni * direction - normal * (ni * nxd + sqrt(k));
		return true;
	}
};
