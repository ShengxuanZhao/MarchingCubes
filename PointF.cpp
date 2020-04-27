#include "PointF.h"

PointF::PointF() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	isosurface = 0;
	in_model = false;
}

PointF::~PointF() {}

PointF::PointF(float x, float y, float z, int i) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->isosurface = i;
}

float PointF::GetX() {
	return x;
}

float PointF::GetY() {
	return y;
}

float PointF::GetZ() {
	return z;
}

int PointF::GetI()
{
	return isosurface;
}

void PointF::SetI(int i)
{
	this->isosurface = i;
}

bool PointF::GetIsInsideSphere() {
	return in_model;
}

void PointF::SetInModel(int target_iso) 
{
	if (target_iso <= isosurface)
	{
		in_model = true;
	}
	else
	{
		in_model = false;
	}
}