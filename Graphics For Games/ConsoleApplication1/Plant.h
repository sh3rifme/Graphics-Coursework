#pragma once
#include "../Framework/SceneNode.h"

class Plant : public SceneNode {
public:
	Plant(float iter);
	virtual ~Plant(void){ delete cylinder; delete leaf; delete sphere; };

	virtual void	Update(float msec);

	static void		CreateBranch();
	static void		CreateLeaf();
	static void		CreateSphere();

	static Mesh*	GenerateSphere() { if (!sphere)CreateSphere(); return sphere; }

	static void		DeleteBranch() { delete cylinder; }
	static void		DeleteLeaf() { delete leaf; }
	static void		DeleteSphere() { delete sphere; }

	void			CreateTree(SceneNode* arg, float branchLen);
	void			ApplyTransform(SceneNode* arg, float branchLen);

protected:
	float			GetHeight(float arg, float mod);

	static Mesh*	cylinder;
	static Mesh*	leaf;
	static Mesh*	sphere;
	SceneNode*		trunk;
	bool			grow;

	float			growRate;
	float			trunkLength;
	float			branchLength;
	int				branches;
	int				listIndex;
};

