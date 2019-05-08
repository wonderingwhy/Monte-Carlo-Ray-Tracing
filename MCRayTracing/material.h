#pragma once
#include "common.h"
class Material {
public:
	bool is_exist;
	string name;
	Vec3f tf;
	Vec3f ka;//������ϵ��
	Vec3f kd;//������ϵ��
	Vec3f ks;//���淴��ϵ��
	Vec3f ke;//����
	Real ns;//�����
	Real ni;//����ϵ��
	Material() {}
	Material(string _name, bool is_exist = false) :name(_name) {}
};
