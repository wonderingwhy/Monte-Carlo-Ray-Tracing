#pragma once
#include "common.h"
class Material {
public:
	bool is_exist;
	string name;
	Vec3f tf;
	Vec3f ka;//环境光系数
	Vec3f kd;//漫反射系数
	Vec3f ks;//镜面反射系数
	Vec3f ke;//发光
	Real ns;//光泽度
	Real ni;//折射系数
	Material() {}
	Material(string _name, bool is_exist = false) :name(_name) {}
};
