#pragma once
#include "ray.h"
#include "material.h"
class Model;
class Intersection {
public:
	Vec3f point;
	Vec3f normal;
	int mtl_id;
	Intersection(Vec3f _point = Vec3f(), Vec3f _normal = Vec3f(), int _mtl_id = -1) :point(_point), normal(_normal), mtl_id(_mtl_id) {}
};
class BoundingBox {
public:
	Vec3f lower_bound, upper_bound;
	BoundingBox(Vec3f _lower = Vec3f(0.0, 0.0, 0.0), Vec3f _upper = Vec3f(0.0, 0.0, 0.0)) :
		lower_bound(_lower), upper_bound(_upper) {};
	bool intersect(Ray& ray, Intersection& intersection);
};
BoundingBox merge(const BoundingBox& box1, const BoundingBox& box2);

class Object {
public:
	int mtl_id;
	Model *model;

	virtual BoundingBox get_box() = 0;
	virtual bool intersect(Ray& ray, Intersection& intersection) = 0;
	virtual bool shadow_ray_intersect(Ray& ray, Intersection& intersection) {
		return intersect(ray, intersection);
	}

};

class Triangle : public Object {
public:
	Vec3i v_id;
	Vec3i vn_id;
	Vec3f edge[2];
	Vec3f normal;
	BoundingBox box;
	Triangle(Model &model, int *_v_id, int *_vn_id, int _mtl_id = -1);
	bool intersect(Ray &ray, Intersection &intersection);
	Vec3f barycentric(const Vec3f &point);
	BoundingBox get_box();
};

enum AXIS { X, Y, Z };
class KdTree : public Object {
public:	
	KdTree();
	KdTree(std::vector<Object*>& geometry_list, int _split_axis = AXIS::Z);
	void build(std::vector<Object*>& geometry_list, int _split_axis = AXIS::Z);
	void partition(int axis, const std::vector<Object*>& all,
		std::vector<Object*>* lower_vec, std::vector<Object*>* upper_vec);
	BoundingBox get_box();
	bool intersect(Ray& ray, Intersection& intersection);
	bool shadow_ray_intersect(Ray& ray, Intersection& intersection);
	int split_axis;//切分的坐标标记，AXIS::XYZ
	Object *lower, *upper;
	BoundingBox box;
};
