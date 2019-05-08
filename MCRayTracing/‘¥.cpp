#include <cstdio>
#include <sstream>
#include <ctime>
#include "model.h"
#include "scene.h"
#include "writer.h"
using namespace std;
int iteration = 0;
const int max_iteration = 10000;
vector<Light> lights;
void render_scene01() {
	Model model("../models/scene01.obj");
	Camera camera;
	int width = 700, height = 600;
	float fov = 40;
	Vec3f center = (model.box.lower_bound + model.box.upper_bound) / 2;
	float scale = (model.box.upper_bound - model.box.lower_bound).length() / 2;
	camera.look_at(center.x, center.y, center.z + 2.3 * scale,
		center.x, center.y, center.z,
		0, 1, 0);
	camera.calc_view_port(fov, width, height);
	Scene scene(model, camera, lights, width, height);
	while (++iteration <= max_iteration) {
		time_t start = clock();
		scene.render();
		time_t end = clock();
		printf("iteration = %d, time = %lf s\n", iteration, (double)(end - start) / 1000);
		draw(scene.image, width, height, "../scene01/", iteration);
	}
}
void render_scene02() {
	Model model("../models/scene02.obj");
	Camera camera;
	int width = 700, height = 600;
	float fov = 30;
	Vec3f center = (model.box.lower_bound + model.box.upper_bound) / 2;
	center += Vec3f(2, -1, 0);
	float scale = (model.box.upper_bound - model.box.lower_bound).length() / 2;
	Vec3f eye(center.x, center.y + 0.5 * scale, center.z + scale);
	camera.look_at(eye.x, eye.y, eye.z,
		center.x, center.y, center.z,
		0, 1, 0);
	camera.calc_view_port(fov, width, height);
	vector<Light> lights;
	Vec3f origin = eye;
	Vec3f dx = Vec3f(10, 0, 0);
	Vec3f dy = Vec3f(0, 0, 10);
	Vec3f emission = Vec3f(100, 100, 100);
	lights.push_back(Light(center + Vec3f(2, 5, 5 + 1.3 * scale), dx, dy, emission, model));
	Scene scene(model, camera, lights, width, height);
	while (++iteration <= max_iteration) {
		time_t start = clock();
		scene.render();
		time_t end = clock();
		printf("iteration = %d, time = %lf s\n", iteration, (double)(end - start) / 1000);
		draw(scene.image, width, height, "../scene02/", iteration);
	}
}

void render_scene03() {
	string obj_name = "scene03.obj";
	Model model("../models/" + obj_name);
	Camera camera;
	int width = 700, height = 600;
	float fov = 60;

	Vec3f center = (model.box.upper_bound + model.box.lower_bound) / 2;
	float scale = (model.box.upper_bound - model.box.lower_bound).length() / 2;
	Vec3f view_point = Vec3f(center.x, center.y, center.z + 1.6 * scale);
	camera.look_at(view_point.x, view_point.y, view_point.z, center.x, center.y, center.z, 0, 1, 0);

	camera.calc_view_port(fov, width, height);
	vector<Light> lights;
	Scene scene(model, camera, lights, width, height);
	while (++iteration <= max_iteration) {
		time_t start = clock();
		scene.render();
		time_t end = clock();
		printf("iteration = %d, time = %lf s\n", iteration, (double)(end - start) / 1000);
		draw(scene.image, width, height, "../scene03/", iteration);
	}
}
int main() {
	srand(time(0));
	render_scene01();
	//render_scene02();
	//render_scene03();
	system("pause");
	return 0;
}