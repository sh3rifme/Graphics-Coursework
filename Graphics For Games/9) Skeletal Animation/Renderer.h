#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/Camera.h"
#include "../Framework/MD5Mesh.h"
#include "../Framework/MD5Node.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	virtual void UpdateScene(float msec);
protected:
	MD5FileData*	hellData;
	MD5Node*		hellNode;
	Camera*			camera;
};