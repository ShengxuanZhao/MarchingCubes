#pragma once
#include<vector>
#include<iostream>
#include<string>
#include<GL/glew.h>
#include<GL/freeglut.h>

using namespace std;
//#pragma  comment(lib, "glew32.lib")

class PointF {
private:
	float x, y, z;				//xyz position
	int isosurface;
	bool in_model;		//in the object or notz

public:
	PointF();
	~PointF();

	PointF(float x, float y, float z, int i);
	float GetX();
	float GetY();
	float GetZ();
	int GetI();
	void SetI(int i);
	bool GetIsInsideSphere();
	void SetInModel(int target_iso);

};