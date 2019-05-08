#include "model.h"
#include "common.h"
#include <fstream>
#include <string>
#include <sstream>  
#include "material.h"
using namespace std;
Model::Model(const string obj_path) {
	string mtl_path;
	if (load_obj(obj_path, mtl_path) == false) {
		cerr << "load obj failed!" << endl;
		return;
	}
	else {
		cerr << "load obj succeed!" << endl;
	}
	if (load_mtl(mtl_path) == false) {
		cerr << "load mtl failed!" << endl;
	}
	else {
		cerr << "load mtl succeed!" << endl;
	}
	kd_tree.build(triangles);
}

bool Model::load_obj(const string obj_path, string &mtl_path) {
	ifstream file(obj_path);
	if (!file.is_open()) {
		cerr << "cannot open " << obj_path << endl;
		return false;
	}
	
	string ss;
	int mtl_id = -1;
	while (getline(file, ss)) {
		istringstream is(ss);
		string type;
		is >> type;
		if (type == "mtllib") {
			is >> mtl_path;
			mtl_path = obj_path.substr(0, obj_path.find_last_of("/") + 1) + mtl_path;
		}
		else if (type == "usemtl") {
			string mtl_name;
			is >> mtl_name;
			mtl_id = -1;
			for (int i = 0; i < materials.size(); ++i) {
				if (materials[i].name == mtl_name) {
					mtl_id = i;
					break;
				}
			}
			if (mtl_id == -1) {
				mtl_id = materials.size();
				materials.push_back(Material(mtl_name));
			}
		}
		else if (type == "v") {
			Real x, y, z;
			is >> x >> y >> z;
			vertexes.push_back(Vec3f(x, y, z));
			box.lower_bound.x = min(x, box.lower_bound.x);
			box.lower_bound.y = min(y, box.lower_bound.y);
			box.lower_bound.z = min(z, box.lower_bound.z);
			box.upper_bound.x = max(x, box.upper_bound.x);
			box.upper_bound.y = max(y, box.upper_bound.y);
			box.upper_bound.z = max(z, box.upper_bound.z);
		}
		else if (type == "vn") {
			Real x, y, z;
			is >> x >> y >> z;
			v_normals.push_back(Vec3f(x, y, z));
		}
		else if (type == "f") {
			string str;
			int cnt = 0;
			int v_id[3] = { 0 }, vn_id[3] = { 0 };
			while(is >> str) {
				int temp;
				int pos = 0;
				while ((pos = str.find("/"))!= -1) {
					str.replace(pos, 1, " ");
				}
				istringstream iss(str);
				iss >> v_id[cnt] >> temp >> vn_id[cnt];
				--v_id[cnt];
				--vn_id[cnt];
				if (++cnt >= 3) {
					Triangle *triangle = new Triangle(*this, v_id, vn_id, mtl_id);
					triangles.push_back(triangle);
					v_id[1] = v_id[2];
					vn_id[1] = vn_id[2];
					cnt = 2;
				}
			}
		}
	}
	file.close();
	return true;
}

bool Model::load_mtl(const string mtl_path) {
	ifstream file(mtl_path);
	if (!file.is_open()) {
		cerr << "cannot open " << mtl_path << endl;
		return false;
	}
	string ss;
	int index = -1;
	while (getline(file, ss)) {
		istringstream is(ss);
		string type;
		is >> type;
		if (type == "newmtl") {
			string mtl_name;
			is >> mtl_name;
			for (int i = 0; i < materials.size(); ++i) {
				if (materials[i].name == mtl_name) {
					materials[i].is_exist = true;
					index = i;
					break;
				}
			}
		}
		else if (index != -1 && type == "Kd") {
			is >> materials[index].kd.x >> materials[index].kd.y >> materials[index].kd.z;
		}
		else if (index != -1 && type == "Ka") {
			is >> materials[index].ka.x >> materials[index].ka.y >> materials[index].ka.z;
		}
		else if (index != -1 && type == "Ks") {
			is >> materials[index].ks.x >> materials[index].ks.y >> materials[index].ks.z;
		}
		else if (index != -1 && type == "Ke") {
			is >> materials[index].ke.x >> materials[index].ke.y >> materials[index].ke.z;
		}
		else if (index != -1 && type == "Ns") {
			is >> materials[index].ns;
		}
		else if (index != -1 && type == "Ni") {
			is >> materials[index].ni;
		}
		else if (index != -1 && type == "Tf") {
			is >> materials[index].tf.x >> materials[index].tf.y >> materials[index].tf.z;
		}
	}
	file.close();
	for (int i = 0; i < materials.size(); ++i) {
		if (materials[i].is_exist == false) {
			cerr << materials[i].name << " is not in " << mtl_path << "!" << endl;
			return false;
		}
	}
	return true;
}
BoundingBox Model::get_box() {
	return kd_tree.get_box();
}

bool Model::intersect(Ray& ray, Intersection& intersection)
{
	return kd_tree.intersect(ray, intersection);
}

bool Model::shadow_ray_intersect(Ray& ray, Intersection& intersection)
{
	return kd_tree.shadow_ray_intersect(ray, intersection);
}
