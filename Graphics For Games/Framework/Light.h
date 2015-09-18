#pragma once

//#include "../Framework/SceneNode.h"
#include "Vector3.h"
#include "Vector4.h"

class Light {
public:
	Light(void){};
	Light(Vector3 position, Vector4 colour, float radius) {
		//sphere = new SceneNode();
		//sphere->SetMesh(Mesh::GenerateSphere(1.0f, 12, 24));
		this->position	= position;
		this->colour	= colour;
		this->radius	= radius;
		//sphere->SetTransform(Matrix4::Translation(position));
	}

	~Light(void){};

	//void		DrawSphere()						{ sphere->Draw(); }

	Vector3		GetPosition()				const	{ return position; }
	void		SetPosition(Vector3 val)			{ position = val; }
													  //sphere->SetTransform(Matrix4::Translation(position)); }

	float		GetRadius()					const	{ return radius; }
	void		SetRadius(float val)				{ radius = val; }

	Vector4		GetColour()					const	{ return colour; }
	void		SetColour(Vector4 val)				{ colour = val; }

protected:
	//SceneNode*	sphere;
	Vector3		position;
	Vector4		colour;
	float		radius;
};