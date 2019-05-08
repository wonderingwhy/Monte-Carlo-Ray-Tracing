#pragma once
#include "ray.h"
#include "light.h"
#include "model.h"
#include "camera.h"
#include "object.h"
class Model;
class Light;
class Camera;
class Scene {
public:
	vector<Light> lights;
	Vec3f* image;
	int width, height;
	int iteration = 0;
	int max_depth = 5;
	Model *model;
	KdTree kd_tree;
	Camera *camera;
	Vec3f ambient = Vec3f(0.1f, 0.1f, 0.1f);
	Scene(Model &_model, Camera &camera, vector<Light> &_lights, int width, int height);
	Vec3f im_sample(Vec3f up, Real ns);
	Ray mc_sample(Ray &ray, Intersection &intersection);
	Vec3f tracing(Ray &ray, int depth = 0);
	bool intersect(Ray &ray, Intersection &intersection);
	bool is_shadow(Ray &ray);
	Real get_fresnel(Real ni, Real nt, Real cos_theta);
	Vec3f direct_light(Intersection& intersection, Ray& ray);
	void render();
};