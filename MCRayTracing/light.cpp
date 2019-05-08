#include "Light.h"

Light::Light(Vec3f _origin, Vec3f _dx, Vec3f _dy, Vec3f _emission, Model &_model) {
	model = &_model;
	light_samples = 3;
	origin = _origin;
	dx = _dx;
	dy = _dy;
	emission = _emission;
	normal = cross(dx, dy);
	area = normal.length();;
	normal = normal.normalize();
}

Vec3f Light::render(Intersection& intersection, Ray& ray, Scene* scene) {
	if (emission == Vec3f()) {
		return emission;
	}
	Vec3f rgb;
	Material& material = model->materials[intersection.mtl_id];
	Real rate = 1.0 / light_samples;

	for (int i = 0; i < light_samples; i++) {
		Real sx = rand() * 1.0 / RAND_MAX;
		Real sy = rand() * 1.0 / RAND_MAX * (1.0 - sx);

		Vec3f light_origin = origin + dx * sx + dy * sy;
		Vec3f r = light_origin - intersection.point;
		Ray shadow_ray = Ray(intersection.point, r);
		shadow_ray.r_max = r.length();

		if (!scene->is_shadow(shadow_ray)) {
			Vec3f s = r.normalize();
			Real cosThetaIn = max(dot(intersection.normal, s), (Real)0.0);
			Real cosThetaOut = max(dot(-s, normal), (Real)0.0);
			Real geoFactor = cosThetaIn * cosThetaOut / sqr(r.length());
			Vec3f intensity = geoFactor * area * emission * rate;

			if (material.kd != Vec3f()) {
				Real mDots = dot(s, intersection.normal);
				if (mDots > 0.0) {
					rgb += mDots * material.kd * intensity / PI;
				}
			}

			if (material.ks != Vec3f()) {
				Vec3f v = -ray.direction;
				Vec3f h = (s + v).normalize();
				Real mDotH = dot(h, intersection.normal);
				if (mDotH > 0.0) {
					rgb += pow(mDotH, material.ns) * material.ks * intensity * (material.ns + 1) / (2 * PI);
				}
			}
		}
	}
	return rgb;
}
