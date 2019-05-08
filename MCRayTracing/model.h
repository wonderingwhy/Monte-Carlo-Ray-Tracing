#pragma once
#include "common.h"
#include "light.h"
#include "material.h"
#include "object.h"
class Light;
class Object;
using namespace std;
class Model : public Object {
public:
	vector<Vec3f> vertexes;
	vector<Vec3f> v_normals;
	vector<Object*> triangles;
	vector<Material> materials;
	KdTree kd_tree;
	BoundingBox box;

	Model() {};
	Model(const string obj_path);

	bool load_obj(const string obj_path, string &mtl_path);
	bool load_mtl(const string mtl_path);
	BoundingBox get_box();
	bool intersect(Ray& ray, Intersection& intersection);
	bool shadow_ray_intersect(Ray& ray, Intersection& intersection);
};