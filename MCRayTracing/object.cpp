#include "ray.h"
#include "object.h"
#include "model.h"

bool BoundingBox::intersect(Ray& ray, Intersection& result)
{
	Real t1 = (lower_bound.x - ray.origin.x) / ray.direction.x;
	Real t2 = (upper_bound.x - ray.origin.x) / ray.direction.x;

	Real t3 = (lower_bound.y - ray.origin.y) / ray.direction.y;
	Real t4 = (upper_bound.y - ray.origin.y) / ray.direction.y;

	Real t5 = (lower_bound.z - ray.origin.z) / ray.direction.z;
	Real t6 = (upper_bound.z - ray.origin.z) / ray.direction.z;

	Real tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	Real tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	if (tmax < 0 || tmin > tmax) {
		return false;
	}
	return tmin < ray.r_max && tmax > ray.r_min;
}
BoundingBox merge(const BoundingBox& box1, const BoundingBox& box2) {
	return BoundingBox(
		Vec3f(min(box1.lower_bound.x, box2.lower_bound.x),
			min(box1.lower_bound.y, box2.lower_bound.y),
			min(box1.lower_bound.z, box2.lower_bound.z)),
		Vec3f(max(box1.upper_bound.x, box2.upper_bound.x),
			max(box1.upper_bound.y, box2.upper_bound.y),
			max(box1.upper_bound.z, box2.upper_bound.z))
	);
}
Triangle::Triangle(Model &_model, int *_v_id, int *_vn_id, int _mtl_id) {
	mtl_id = _mtl_id;
	this->model = &_model;
	memcpy(&v_id, _v_id, sizeof(Vec3i));
	memcpy(&vn_id, _vn_id, sizeof(Vec3i));
	Vec3f p0 = model->vertexes[v_id.x];
	Vec3f p1 = model->vertexes[v_id.y];
	Vec3f p2 = model->vertexes[v_id.z];
	edge[0] = p1 - p0;
	edge[1] = p2 - p0;
	normal = cross(edge[0], edge[1]).normalize();
	box = get_box();
}
Vec3f Triangle::barycentric(const Vec3f &point) {
	Vec3f p0 = model->v_normals[vn_id.x];
	Vec3f p1 = model->v_normals[vn_id.y];
	Vec3f p2 = model->v_normals[vn_id.z];
	Vec3f temp_p = point - model->vertexes[v_id.x];
	Real d00 = dot(edge[0], edge[0]);
	Real d01 = dot(edge[0], edge[1]);
	Real d11 = dot(edge[1], edge[1]);
	Real d20 = dot(temp_p, edge[0]);
	Real d21 = dot(temp_p, edge[1]);
	Real d = d00 * d11 - d01 * d01;
	Real v = (d11 * d20 - d01 * d21) / d;
	Real w = (d00 * d21 - d01 * d20) / d;
	Real u = 1 - v - w;
	v = fabs(v);
	w = fabs(w);
	u = fabs(u);
	return (u * p0 + v * p1 + w * p2).normalize();
}
BoundingBox Triangle::get_box() {
	Vec3f p0 = model->vertexes[v_id.x];
	Vec3f p1 = model->vertexes[v_id.y];
	Vec3f p2 = model->vertexes[v_id.z];
	BoundingBox temp_box;
	temp_box.lower_bound = Vec3f(min(p0.x, min(p1.x, p2.x)), min(p0.y, min(p1.y, p2.y)), min(p0.z, min(p1.z, p2.z)));
	temp_box.upper_bound = Vec3f(max(p0.x, max(p1.x, p2.x)), max(p0.y, max(p1.y, p2.y)), max(p0.z, max(p1.z, p2.z)));
	return temp_box;
}
bool Triangle::intersect(Ray &ray, Intersection &intersection) {
	if (abs(dot(normal, ray.direction)) < EPS) {
		return false;
	}
	Vec3f p0 = model->vertexes[v_id.x];
	Vec3f v = p0 - ray.origin;
	Vec3f tmp1 = cross(edge[1], ray.direction);
	Real beta = -dot(tmp1, v) / dot(tmp1, edge[0]);

	Vec3f tmp2 = cross(edge[0], ray.direction);
	Real gamma = -dot(tmp2, v) / dot(tmp2, edge[1]);

	Real t = dot(normal, v) / dot(normal, ray.direction);

	if (beta > 0 && gamma > 0 && beta + gamma < 1 && ray.is_in(t)) {
		intersection.point = ray.reach_point(t);
		intersection.normal = barycentric(intersection.point);
		intersection.mtl_id = this->mtl_id;
		ray.r_max = t;
		return true;
	}
	return false;
}

KdTree::KdTree() :lower(NULL), upper(NULL) { }
KdTree::KdTree(vector<Object*>& geometry_list, int _split_axis) { build(geometry_list, _split_axis); }

void KdTree::build(vector<Object*>& geometry_list, int _split_axis) {
	split_axis = _split_axis;
	unsigned int length = geometry_list.size();

	if (length == 1) {
		lower = geometry_list[0];
		upper = NULL;
		box = lower->get_box();
	}
	else if (length == 2) {
		lower = geometry_list[0];
		upper = geometry_list[1];
		box = merge(lower->get_box(), upper->get_box());
	}
	else {
		vector<Object*> lower_vec;
		vector<Object*> upper_vec;
		partition(split_axis, geometry_list, &lower_vec, &upper_vec);

		lower = lower_vec.size() > 0 ? new KdTree(lower_vec, (split_axis + 1) % 3) : NULL;
		upper = upper_vec.size() > 0 ? new KdTree(upper_vec, (split_axis + 1) % 3) : NULL;

		if (lower == NULL) {
			box = upper->get_box();
		}
		else if (upper == NULL) {
			box = lower->get_box();
		}
		else {
			box = merge(lower->get_box(), upper->get_box());
		}
	}
}


void KdTree::partition(int axis, const vector<Object*>& all, vector<Object*>* lower_vec, vector<Object*>* upper_vec) {

	Real min_v, max_v;
	min_v = INF;
	max_v = -INF;

	for (unsigned int i = 0; i < all.size(); i++) {
		BoundingBox temp_box = all[i]->get_box();
		min_v = min(min_v, temp_box.lower_bound.num[axis]);
		max_v = max(max_v, temp_box.upper_bound.num[axis]);
	}

	Real pivot = (min_v + max_v) / 2;

	for (unsigned int i = 0; i < all.size(); i++) {
		BoundingBox temp_box = all[i]->get_box();
		Real center_c = (temp_box.upper_bound.num[axis] + temp_box.lower_bound.num[axis]) / 2;
		if (center_c < pivot) {
			lower_vec->push_back(all[i]);
		}
		else {
			upper_vec->push_back(all[i]);
		}
	}

	if (upper_vec->size() == all.size()) {
		Real min_c = FLT_MAX;
		unsigned int index;
		Object* obj;
		for (unsigned int i = 0; i < upper_vec->size(); i++) {
			BoundingBox temp_box = (*upper_vec)[i]->get_box();
			Real center_c = (temp_box.upper_bound.num[axis] + temp_box.lower_bound.num[axis]) / 2;
			if (center_c < min_c) {
				min_c = center_c;
				index = i;
				obj = (*upper_vec)[i];
			}
		}
		upper_vec->erase(upper_vec->begin() + index);
		lower_vec->push_back(obj);
	}
	else if (lower_vec->size() == all.size()) {
		Real maxCoord = -FLT_MAX;
		unsigned int index;
		Object* obj;
		for (unsigned int i = 0; i < lower_vec->size(); i++) {
			BoundingBox temp_box = (*lower_vec)[i]->get_box();
			Real center_c = (temp_box.upper_bound.num[axis] + temp_box.lower_bound.num[axis]) / 2;
			if (center_c > maxCoord) {
				maxCoord = center_c;
				index = i;
				obj = (*lower_vec)[i];
			}
		}
		lower_vec->erase(lower_vec->begin() + index);
		upper_vec->push_back(obj);
	}
}


BoundingBox KdTree::get_box() { return box; }

bool KdTree::intersect(Ray& ray, Intersection& intersection) {

	if (!box.intersect(ray, intersection)) 
		return false;

	bool hit = false;

	if (ray.direction.num[split_axis] >= 0) {
		if (lower != NULL) {
			hit |= lower->intersect(ray, intersection);
		}
		if (upper != NULL) {
			hit |= upper->intersect(ray, intersection);
		}
	}
	else {
		if (upper != NULL) {
			hit |= upper->intersect(ray, intersection);
		}
		if (lower != NULL) {
			hit |= lower->intersect(ray, intersection);
		}
	}
	return hit;
}

bool KdTree::shadow_ray_intersect(Ray& ray, Intersection& intersection) {
	if (!box.intersect(ray, intersection)) return false;

	bool hit = false;

	if (ray.direction.num[split_axis] >= 0) {
		if (lower != NULL) {
			hit |= lower->shadow_ray_intersect(ray, intersection);
		}
		if (!hit&&upper != NULL) {
			hit |= upper->shadow_ray_intersect(ray, intersection);
		}
	}
	else {
		if (upper != NULL) {
			hit |= upper->shadow_ray_intersect(ray, intersection);
		}
		if (!hit&&lower != NULL) {
			hit |= lower->shadow_ray_intersect(ray, intersection);
		}
	}
	return hit;
}
