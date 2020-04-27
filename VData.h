#pragma once
#include"PointF.h"

class Voxel {
private:
    PointF iVertex[8];						//8 vertices
    int iIndex;								//index used in EdgeTable and TriTable
    PointF iIntersectVertex[12];			//12 edges intersected with iso-surface

public:
    Voxel();
    ~Voxel();
    void SetVertex(PointF p, int index);
    PointF GetVertex(int index);
    void CalculateiIndex();
    void CalculateIntersectVertex();

    void DrawWireCubes();
    void DrawISOSurface();

};