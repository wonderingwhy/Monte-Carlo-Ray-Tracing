#pragma once
#include "common.h"
#include "object.h"
#include "scene.h"
#include "model.h"
class Scene;
class Model;
class Light {
public:
	Vec3f origin;
	Vec3f dx, dy;
	Vec3f normal;
	Vec3f emission;
	Real area;
	Model *model;
	int light_samples;
	Light(Vec3f _origin, Vec3f _dx, Vec3f _dy, Vec3f _emission, Model &_model);
	Vec3f render(Intersection& intersection, Ray& ray, Scene* scene);
};