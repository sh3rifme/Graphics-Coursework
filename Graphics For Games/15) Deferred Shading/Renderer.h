#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/Camera.h"
#include "../Framework/OBJmesh.h"
#include "../Framework/HeightMap.h"
#include "../Framework/Light.h"

#define LIGHTNUM 8 //Generate LIGHTNUM^2 lights.

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint &into, bool depth = false);

	Shader*		sceneShader; // Shader to fill our GBuffers
    Shader*		pointlightShader; // Shader to calculate lighting
    Shader*		combineShader; // shader to stick it all together
    
    Light*		pointLights; // Array of lighting data
    Mesh*		heightMap; // Terrain !
    OBJMesh*	sphere; // Light volume
    Mesh*		quad; // To draw a full - screen quad
    Camera*		camera; // Our usual camera
    
    float		rotation; // How much to increase rotation by
    
    GLuint		bufferFBO; // FBO for our G- Buffer pass
    GLuint		bufferColourTex; // Albedo goes here
    GLuint		bufferNormalTex; // Normals go here
    GLuint		bufferDepthTex; // Depth goes here
    
    GLuint		pointLightFBO; // FBO for our lighting pass
    GLuint		lightEmissiveTex; // Store emissive lighting
    GLuint		lightSpecularTex; 
};

