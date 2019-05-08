#pragma once
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include "common.h"
using namespace std;
const int BUFFERSIZE = 2000 * 2000;
unsigned char BufferR[BUFFERSIZE], BufferG[BUFFERSIZE], BufferB[BUFFERSIZE];

void cleanframe() {
	memset(BufferR, 0, sizeof(BufferR));
	memset(BufferG, 0, sizeof(BufferG));
	memset(BufferB, 0, sizeof(BufferB));
}
void output(int px, int py, string fname) {
	unsigned char head_buf[100] = { 66,77,54,8,7,0,0,0,0,0,54,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,1,0,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	FILE *fout = fopen(fname.c_str(), "wb");
	int *xlenp = (int *)&head_buf[18], *ylenp = (int *)&head_buf[22];
	int off = (4 - (px * 3 % 4)) % 4;
	unsigned char Zero[] = { 0,0,0,0 };
	*xlenp = px; *ylenp = py;
	fwrite(head_buf, 54, 1, fout);
	for (int i = 0; i < px * py; ++i) {
		fwrite(&BufferB[i], 1, 1, fout);
		fwrite(&BufferG[i], 1, 1, fout);
		fwrite(&BufferR[i], 1, 1, fout);
		if (i && i % px == px - 1 && off) {
			fwrite(Zero, off, 1, fout);
		}
	}
	fclose(fout);
}
string int2str(const int &int_temp) {
	stringstream stream;
	stream << int_temp;
	return stream.str();  
}
unsigned char resize(float v) {
	if (v > 255) {
		return 255;
	}
	if (v < 0) {
		return 0;
	}
	return (unsigned char)v;
}
void draw(Vec3f* image, int width, int height, string path_name, int iteration) {
	cleanframe();
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int pos = y * width + x;
			BufferR[pos] = resize(image[pos].r * 255 + 0.5);
			BufferG[pos] = resize(image[pos].g * 255 + 0.5);
			BufferB[pos] = resize(image[pos].b * 255 + 0.5);
		}
	}
	output(width, height, path_name + int2str(iteration) + ".bmp");
}