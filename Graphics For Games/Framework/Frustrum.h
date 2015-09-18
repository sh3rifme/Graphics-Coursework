#pragma once

#include "Plane.h"
#include "Matrix4.h"
#include "SceneNode.h"

class Matrix4;

class Frustrum {
public:
	Frustrum(void) {};
	~Frustrum(void) {};

	void		FromMatrix(const Matrix4 &mvp);
	bool		InsideFrustrum(SceneNode &n);
protected:
	Plane planes[6];
};