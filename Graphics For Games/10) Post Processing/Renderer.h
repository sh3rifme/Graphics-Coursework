#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/HeightMap.h"
#include "../Framework/Camera.h"

#define POST_PASSES 1

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	Shader* sceneShader;
	Shader* processShader;

	Camera* camera;

	Mesh* quad;
	HeightMap* heightMap;

	GLuint	bufferFBO;
	GLuint	processFBO;
	GLuint	bufferColourTex[2];
	GLuint	bufferDepthTex;
};

