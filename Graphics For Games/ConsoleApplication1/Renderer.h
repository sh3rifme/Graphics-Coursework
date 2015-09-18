#pragma once

#include "../Framework/Camera.h"
#include "../Framework/OBJmesh.h"
#include "../Framework/Frustrum.h"
#include "../Framework/HeightMap.h"
#include "../Framework/TextMesh.h"
#include "Weather.h"
#include "Plant.h"

#define BLUR_PASSES 2
#define SHADOWSIZE 2048*8 
#define LIGHTNUM 8

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void		RenderScene();
	virtual void		UpdateScene(float msec);

	void				Toggle(int arg_num);
	void				IncBlur(bool pos);
	void				ResetScene();

protected:
	void				DrawNode(SceneNode* n);
	void				DrawScene();
	void				PresentScene();
	void				DrawPostProcess();

	void				EdgeDetectPass();
	void				BlurPass();
	void				DrawWater();
	void				ShadowPass();
	void				PointLightPass();
	void				DrawParticles();
	
	void				DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective);

	string				GetWeatherType();
	int					GetTreeMemUsage();
	void				SetShaderParticleSize(float f);
	void				GenerateScreenTexture(GLuint &into, bool depth = false);
	bool				ActiveTex();
	bool				LoadCheck();
	void				RotateLight();
	string				IsOn(bool arg);

	Camera*				camera;
	SceneNode*			sceneNode;
	SceneNode*			waterNode;
	SceneNode*			lightSphere;
	HeightMap*			heightMap;
	Light*				light;
	Light*				castLight;
	Light*				pointLights;

	Weather*			snow;
	Weather*			rain;


	OBJMesh*			sphere;
	Mesh*				cylinder;
	Mesh*				quad;
	Mesh*				waterQuad;

	Shader*				sceneShader;
	Shader*				celShader;
	Shader*				blurShader;
	Shader*				reflectShader;
	Shader*				skyBoxShader;
	Shader*				shadowShader;
	Shader*				pointLightShader;
	Shader*				combineShader;
	Shader*				simpleShader;
	Shader*				particleShader;
	
	Frustrum			frameFrustrum;

	Font*				tahoma;

	GLuint				bufferFBO;
	GLuint				processFBO;
	GLuint				pointLightFBO;
	GLuint				shadowFBO;
	GLuint				bufferColourTex[2];
	GLuint				bufferDepthTex;
	GLuint				cubeMap;
	GLuint				shadowTex;
	GLuint				lightEmissiveTex;
	GLuint				lightSpecularTex;
	GLuint				bufferNormalTex;

	bool				celShade;
	bool				blur;
	bool				postProc;
	bool				activeTex;
	bool				lightSpheres;
	bool				pointLight;
	bool				orbit;
	bool				grow;
	bool				help;
	bool				weatherOn;
	bool				weatherSwap;

	int					frameCount;
	int					FPS;
	int					blurPasses;
	int					outBuffer;
	float				deltaTime;
	float				waterRotate;
	float				lightRotation;
};

