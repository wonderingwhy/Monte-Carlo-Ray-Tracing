#include "scene.h"

using namespace std;

Scene::Scene(Model &_model, Camera &_camera, vector<Light> &_lights, int _width, int _height): width(_width), height(_height) {
	lights = _lights;
	image = new Vec3f[height * width];
	model = &_model;
	camera = &_camera;
	vector<Object*> vct;
	vct.push_back(model);
	kd_tree.build(vct);
}
void Scene::render() {
	++iteration;
#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Real dx = rand() * 1.0 / RAND_MAX;
			Real dy = rand() * 1.0 / RAND_MAX;
			Ray ray = camera->get_ray((x + dx) / width, (y + dy) / height);
			Vec3f color = tracing(ray);
			image[y * width + x] = (image[y * width + x] * (iteration - 1) + color) / iteration;
		}
	}
}
bool Scene::intersect(Ray& ray, Intersection& intersection) {
	return kd_tree.intersect(ray, intersection);
}
bool Scene::is_shadow(Ray& ray) {
	Intersection intersection;
	return kd_tree.shadow_ray_intersect(ray, intersection);
}

Vec3f Scene::im_sample(Vec3f up, Real ns) {
	Real r1 = rand() * 1.0 / RAND_MAX, r2 = rand() * 1.0 / RAND_MAX;
	Real phi = r1 * 2 * PI;
	Real theta = ns < 0 ? asin(sqrt(r2)) : acos(pow(r2, 1 / (ns + 1)));
	Vec3f sample(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
	Vec3f front, right;
	front = (fabs(up.x) > fabs(up.y) ? Vec3f(up.z, 0, -up.x) : Vec3f(0, -up.z, up.y));
	front = front.normalize();
	right = cross(up, front);
	return (sample.x * right + sample.y * up + sample.z * front).normalize();
}
Real Scene::get_fresnel(Real ni, Real nt, Real cos_theta) {
	Real f0 = sqr((ni - nt) / (ni + nt));
	Real schlick = f0 + (1 - f0)* pow(1.0 - cos_theta, 5);
	return schlick;
}
Ray Scene::mc_sample(Ray &ray, Intersection &intersection) {
	Vec3f direction;
	Material &material = model->materials[intersection.mtl_id];
	Real n0 = dot(material.kd, Vec3f(1, 1, 1));
	Real n1 = dot(material.ks, Vec3f(1, 1, 1)) + n0;
	if (material.ni!= 1.0)
	{
		Real ni0, ni1;
		Real cos_theta = dot(ray.direction, intersection.normal);
		Vec3f normal = cos_theta <= 0.0 ? intersection.normal : -intersection.normal;

		if (cos_theta > 0.0) {
			ni0 = material.ni;
			ni1 = 1.0;
		}
		else {
			ni0 = 1.0;
			ni1 = material.ni;
		}
		Real fresnel = get_fresnel(ni0, ni1, fabs(cos_theta));
		Real transmissonSurvival;
		if(rand() * 1.0 / RAND_MAX > fresnel) {
			if (ray.refract(normal, ni0 / ni1, direction)) {
				return Ray(intersection.point, direction, SOURCE::REFRACTION);
			}
			else {
				direction = ray.reflect(normal);
				return Ray(intersection.point, direction, SOURCE::SPECULAR);
			}
		}
	}
	if (rand() * 1.0 / RAND_MAX > n0 / n1) {
		Vec3f direction = im_sample(ray.reflect(intersection.normal), material.ns);
		return Ray(intersection.point, direction, SOURCE::SPECULAR);
	}
	else {
		return Ray(intersection.point, im_sample(intersection.normal, -1), SOURCE::DIFFUSE);
	}
}
Vec3f Scene::tracing(Ray &ray, int depth) {
	Intersection intersection;
	if (intersect(ray, intersection) == false) {
		return Vec3f();
	}
	else {
		Material material = model->materials[intersection.mtl_id];
		if(depth >= max_depth) {
			if (material.ke != Vec3f()) {
				return material.ke;
			}
			return Vec3f();
		}
		Vec3f indirect;
		Ray new_ray = mc_sample(ray, intersection);
		if (new_ray.source != SOURCE::NONE) {
			indirect = tracing(new_ray, depth + 1);
			switch (new_ray.source) {
			case DIFFUSE:
				indirect *= material.kd; 
				break;
			case SPECULAR:
				indirect *= material.ks; 
				break;
			default:
				break;
			}
		}
		Vec3f direct = direct_light(intersection, ray);
		return material.ke + indirect + direct + material.ka * ambient;
	}
}
Vec3f Scene::direct_light(Intersection& intersection, Ray& ray) {
	Vec3f color;
	for (int i = 0; i < lights.size(); ++i) {
		color += lights[i].render(intersection, ray, this);
	}
	return color;
}
