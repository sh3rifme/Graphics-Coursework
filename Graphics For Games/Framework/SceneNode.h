#pragma once
#include "../Framework/Mesh.h"

class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1,1,1,1));
	~SceneNode(void);

	float			GetBoundingRadius()				const	{ return boundingRadius; }
	void			SetBoundingRadius(float f)				{ boundingRadius = f; }

	float			GetCameraDistance()				const	{ return distanceFromCamera; }
	void			SetCameraDistance(float f)				{ distanceFromCamera = f; }

	static bool		CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	void			SetTransform(const Matrix4 &matrix)		{ transform = matrix; }
	const Matrix4&	GetTransform()					const	{ return transform; }
	Matrix4			GetWorldTransform()				const	{ return worldTransform; }

	Vector4			GetColour()						const	{ return colour; }
	void			SetColour(Vector4 c)					{ colour = c; }

	Vector3			GetModelScale()					const	{ return modelScale; }
	void			SetModelScale(Vector3 s)				{ modelScale = s; }

	Mesh*			GetMesh()						const	{ return mesh; }
	void			SetMesh(Mesh* m)						{ mesh = m; }

	int				GetMemUsage()							{ return memUsage / 1024; }

	unsigned int	GetNumChild()							{ return children.size(); }
	SceneNode*		GetChildAt(int loc)						{ return children.at(loc); }
	
	std::vector<SceneNode*> GetChildren()					{ return children; }

	void			AddChild(SceneNode* s);

	virtual void	Update(float msec);
	virtual void	Draw();

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin(); }

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end(); }

protected:
	SceneNode*				parent;
	Mesh*					mesh;
	Matrix4					worldTransform;
	Matrix4					transform;
	Vector3					modelScale;
	Vector4					colour;
	std::vector<SceneNode*>	children;
	float					distanceFromCamera;
	float					boundingRadius;
	int						memUsage;
};
