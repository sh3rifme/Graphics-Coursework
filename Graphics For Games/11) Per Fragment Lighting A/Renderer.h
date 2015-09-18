#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/Camera.h"
#include "../Framework/HeightMap.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	Mesh*	heightMap;
	Camera*	camera;
	Light*	light;
};