#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/Camera.h"
#include "../Framework/SceneNode.h"
#include "../Framework/Frustrum.h"
#include "CubeRobot.h"
#include <algorithm>

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void		BuildNodeLists(SceneNode* from);
	void		SortNodeLists();
	void		ClearNodeLists();
	void		DrawNodes();
	void		DrawNode(SceneNode* n);

	SceneNode*	root;
	Camera*		camera;
	Mesh*		quad;

	Frustrum	frameFrustrum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};