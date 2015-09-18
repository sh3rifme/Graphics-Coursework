#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	camera = new Camera;
	camera->SetPosition(Vector3(2000, 300, 2800));
	
	quad		= Mesh::GenerateQuad();

	waterQuad	= Mesh::GenerateQuad();
	waterQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cylinder	= Mesh::GenerateCylinder(50, 20);
	cylinder->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"bark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	heightMap	= new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"grass.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"grassDOT3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	sphere		= new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"sphere.obj"))
		return;

	sceneShader			= new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	celShader			= new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"FreiChenFragment.glsl");
	blurShader			= new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"GaussianFragment.glsl");
	reflectShader		= new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"ReflectFragment.glsl");
	skyBoxShader		= new Shader(SHADERDIR"SkyBoxVertex.glsl", SHADERDIR"SkyBoxFragment.glsl");
	shadowShader		= new Shader(SHADERDIR"ShadowVertex.glsl", SHADERDIR"ShadowFragment.glsl");
	pointLightShader	= new Shader(SHADERDIR"pointLightVertex.glsl", SHADERDIR"pointLightFragment.glsl");
	combineShader		= new Shader(SHADERDIR"combineVert.glsl", SHADERDIR"combineFrag.glsl");
	simpleShader		= new Shader(SHADERDIR"QuadVertex.glsl", SHADERDIR"QuadFrag.glsl");
	particleShader		= new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");

	light				= new Light(Vector3(0, 1000, 0), 
								Vector4(1,1,1,1),
								5500.0f);

	/*lightSphere = new SceneNode(Mesh::GenerateSphere(1, 10, 20), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	lightSphere->SetModelScale(Vector3(10, 10, 10));
	lightSphere->SetTransform(Matrix4::Translation(Vector3(1, 1000, 1)));
*/
	snow = new Weather(Vector3((RAW_HEIGHT * HEIGHTMAP_X), 1500, (RAW_HEIGHT * HEIGHTMAP_Z)));
	snow->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"snowflake.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT));
	snow->SetParticleSpeed(0.05f);
	snow->SetParticleSize(5.0f);
	snow->SetParticleVariance(20.0f);
	snow->SetLaunchParticles(50.0f);
	snow->SetParticleLifetime(30000.0f);

	rain = new Weather(Vector3((RAW_HEIGHT * HEIGHTMAP_X), 1500, (RAW_HEIGHT * HEIGHTMAP_Z)));
	rain->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"raindrop.png",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT));
	rain->SetParticleSpeed(0.6f);
	rain->SetParticleSize(5.0f);
	rain->SetParticleVariance(20.0f);
	rain->SetLaunchParticles(400.0f);
	rain->SetParticleLifetime(5000.0f);

	weatherSwap = false;

	pointLights		= new Light[LIGHTNUM * LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light &l = pointLights[(x * LIGHTNUM) + z];
	
			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM -1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM -1)) * z;
			l.SetPosition(Vector3(xPos ,100.0f, zPos));
	
		float r = 0.5f + (float)(rand()%129) / 128.0f;
		float g = 0.5f + (float)(rand()%129) / 128.0f;
		float b = 0.5f + (float)(rand()%129) / 128.0f;
		l.SetColour(Vector4(r, g, b, 1.0f));
	
		float radius = (RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM);
		l.SetRadius(radius);
		}
	}

	cubeMap			= SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg",TEXTUREDIR"rusted_east.jpg",
											TEXTUREDIR"rusted_up.jpg",TEXTUREDIR"rusted_down.jpg",
											TEXTUREDIR"rusted_south.jpg",TEXTUREDIR"rusted_north.jpg",
											SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	tahoma			= new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);
	
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	
	if (!LoadCheck())
		return;

	//This value determines the number of recursive calls used to generate the tree.
	//Also, it determines the length of each branch. Yay for recursion.
	float iterNum = 6.0f;

	//Three trees because 1 wasn't enough.
	sceneNode = new SceneNode();
	Plant* plant = new Plant(iterNum);
	plant->SetTransform(Matrix4::Translation(Vector3(3600, 200, 3600)));
	sceneNode->AddChild(plant);

	plant = new Plant(iterNum - 1.0f);
	plant->SetTransform(Matrix4::Translation(Vector3(1300, 220, 1000)));
	sceneNode->AddChild(plant);

	plant = new Plant(iterNum + 1.0f);
	plant->SetTransform(Matrix4::Translation(Vector3(1300, 180, 2000)));
	sceneNode->AddChild(plant);

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	SetTextureRepeating(cylinder->GetTexture(),	 true);
	SetTextureRepeating(waterQuad->GetTexture(), true);

	activeTex = 0;
	waterRotate = 0.0f;
	blurPasses = BLUR_PASSES;

	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	//Generate scene depth texture.
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	//And colour texture.
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	//And shadow texture.
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &bufferFBO); //Render the scene into this.
	glGenFramebuffers(1, &processFBO); //PP in this.
	glGenFramebuffers(1, &shadowFBO); //Shadow pre-render in this one.
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	//glDrawBuffers(2, buffers);

	//Check FBO attachment success with this command
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !lightEmissiveTex || !lightSpecularTex)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);

	//Set effects booleans to false (and some true)
	celShade	= false;
	blur		= false;
	lightSpheres= false;
	pointLight	= false;
	orbit		= true;
	grow		= false;
	help		= true;

	FPS			= 0;
	frameCount	= 0;
	deltaTime	= 0.0f;
	lightRotation = 0.5f;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer(void) {
	//Delete meshes
	delete cylinder;
	delete quad;
	delete waterQuad;
	delete sphere;

	//Delete Shaders
	delete sceneShader;
	delete celShader;
	delete blurShader;
	delete reflectShader;
	delete skyBoxShader;
	delete shadowShader;
	delete pointLightShader;

	//Delete lights
	delete light;
	delete[] pointLights;

	//Delete other components.
	delete camera;
	delete heightMap;
	delete sceneNode;

	delete rain;
	delete snow;

	currentShader = NULL;

	//Clear buffers
	glDeleteTextures(1, &shadowTex);
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

/*-------------------------------Public Methods-------------------------------*/
void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix	= camera->BuildViewMatrix();
	if (grow)
	sceneNode->Update(msec);

	waterRotate += msec / 1000.0f;

	if (orbit) 
		RotateLight();

	frameCount++;
	deltaTime += msec;

	if (deltaTime >= 1000.0f) {
		if (FPS > 0) {
			FPS = (frameCount + FPS)/2;
		}
		else {
			FPS = frameCount;
		}
		frameCount = 0;
		deltaTime = 0.0f;
	}
	if (weatherSwap)
		rain->Update(msec);
	else
		snow->Update(msec);
}

void Renderer::RenderScene() {
	DrawScene();
	DrawPostProcess();
	PresentScene();
	SwapBuffers();
}

void Renderer::IncBlur(bool pos) {
	if (pos)
		++blurPasses;
	else if (blurPasses > BLUR_PASSES)
		--blurPasses;
}

void Renderer::Toggle(int arg_num) {
	switch (arg_num)
	{
	case (1):
		celShade = !celShade;
		break;
	case (2):
		blur = !blur;
		break;
	case (3):
		//pointLight = !pointLight;
		break;
	case (4):
		weatherOn = !weatherOn;
		break;
	case (5):
		weatherSwap = !weatherSwap;
		break;
	case (6):
		orbit = !orbit;
		break;
	case (10):
		grow = !grow;
		break;
	case (11):
		help = !help;
		break;
	default:
		break;
	}
}

void Renderer::ResetScene() {
	sceneNode->~SceneNode();
	sceneNode = new Plant(8.0f);
	sceneNode->SetTransform(Matrix4::Translation(Vector3(2300, 220, 2000)));
}

/*-------------------------------Protected Methods-------------------------------*/

/*--Rendering pipeline--*/

//Generate shadows for scene.
void Renderer::ShadowPass() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);

	SetCurrentShader(shadowShader);
	projMatrix		= Matrix4::Perspective(300.0f, 10000.0f, (float)width / (float)height, 45.0f);
	viewMatrix		= Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(2056, 10, 2056));
	shadowMatrix	= biasMatrix*(projMatrix * viewMatrix);

	UpdateShaderMatrices();

	heightMap->Draw();
	DrawNode(sceneNode);
	DrawWater();

	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	//glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Scene is drawn to the buffer object.
void Renderer::DrawScene() {
	ShadowPass();
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	
	//First the Skybox is drawn.
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyBoxShader);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

	//Then the terrain and tree is drawn.
	SetCurrentShader(sceneShader);
	SetShaderLight(*light);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 9);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	//viewMatrix = camera->BuildViewMatrix();
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	UpdateShaderMatrices();

	heightMap->Draw();

	if (lightSpheres) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		DrawNode(lightSphere);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
	
	DrawNode(sceneNode);
	DrawWater();

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	activeTex = false;
	
	glUseProgram(0);
	if (weatherOn)
		DrawParticles();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Next, if post-processing features are enabled, their methods are invoked.
void Renderer::DrawPostProcess() {
	if (celShade) 
		EdgeDetectPass();
	
	if (blur) 
		BlurPass();

	if (pointLight)
		PointLightPass();
}

//Finally the scene is drawn to the screen, using the appropriate buffer.
void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	SetCurrentShader(simpleShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	quad->SetTexture(bufferColourTex[activeTex]);
	quad->Draw();

	if (help) {
		int spacing = 20;
		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		DrawText("Trees Memory Usage: " + to_string(GetTreeMemUsage()) + "KB", Vector3(0,0,0), 16.0f, false);
		DrawText("FPS: " + to_string(FPS), Vector3(0,spacing,0), 16.0f, false);
		DrawText("Press 'G' to grow tree, 'R' to reset tree", Vector3(0, spacing*2, 0), 16.0f, false);
		DrawText("1: Edge Detection (" + IsOn(celShade) + ")", Vector3(0, spacing*3, 0), 16.0f, false);
		DrawText("2: Gaussian Blur (" + IsOn(blur) + ") Passes: " + to_string(blurPasses), Vector3(0, spacing*4, 0), 16.0f, false);
		DrawText("4: Toggle weather ON/OFF (" + IsOn(weatherOn) + ")", Vector3(0, spacing*6, 0), 16.0f, false);
		DrawText("5: Rain/Snow swap (" + GetWeatherType() + ")", Vector3(0, spacing*7, 0), 16.0f, false);
		DrawText("6: Moving Light (" + IsOn(orbit) + ")", Vector3(0, spacing*8, 0), 16.0f, false);
		//glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	glUseProgram(0);
}

//This will draw each SceneNode nested in the root, along with the root.
void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&transform);
		textureMatrix.ToIdentity();
		n->Draw();
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}

/*--Post-Processing passes--*/

//Using Frei-Chen edge-detection with the z-buffer to create a cel-shaded effect.
void Renderer::EdgeDetectPass(){
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[ActiveTex()], 0);//1
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(celShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / (float)width, 1.0f / (float)height);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "zNear"), 0);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "zFar"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 5);

	quad->SetTexture(bufferColourTex[ActiveTex()]);//0
	quad->Draw();
	activeTex = !activeTex;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

//Gaussian blur with adjustable number of passes.
void Renderer::BlurPass() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[ActiveTex()], 0);//1
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(blurShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / (float)width, 1.0f / (float)height);

	for (int i = 0; i < blurPasses; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[activeTex], 0);//1
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		quad->SetTexture(bufferColourTex[ActiveTex()]);//0
		quad->Draw();

		
		//now to swap the colour buffers, and do the second blur Pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[activeTex], 0);//0

		quad->SetTexture(bufferColourTex[ActiveTex()]);//1
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

//Generates a large number of point lights, DISCO DISCO GOOD GOOD. Also Broken :(
void Renderer::PointLightPass() {
	SetCurrentShader(pointLightShader);
	
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBlendFunc(GL_ONE, GL_ONE);
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 6);
	
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1,(float*)&camera->GetPosition());
	
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
	
	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);
	
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light & l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();
	
			modelMatrix = pushMatrix *	Matrix4::Rotation(lightRotation, Vector3(0, 1, 0)) * popMatrix *
										Matrix4::Translation(l.GetPosition()) *
										Matrix4::Scale(Vector3(radius, radius, radius));
	
			l.SetPosition(modelMatrix.GetPositionVector());
	
			SetShaderLight(l);
	
			UpdateShaderMatrices();
	
			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			if (dist < radius) {// camera is inside the light volume !
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}
	
			sphere->Draw();
		}
	}
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[ActiveTex()], 0);
	SetCurrentShader(combineShader);
	
	projMatrix = Matrix4::Orthographic(-1,1,1,-1,-1,1);
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 7);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 8);
	
	quad->SetTexture(bufferColourTex[ActiveTex()]);
	
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);
	
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	
	quad->Draw();
	
	activeTex = !activeTex;

	glUseProgram(0);
}

/*--Methods to neaten up other methods--*/

//Flips the active texture boolean to allow switching of buffers easily.
bool Renderer::ActiveTex() {
	activeTex = !activeTex;
	return activeTex;
}

//Draws the water quad into the scene.
void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"),1,(float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"cubeTex"), 3);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	
	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
	
	modelMatrix =	Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
					Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
					Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
					Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	
	UpdateShaderMatrices();
	waterQuad->Draw();
	glUseProgram(0);
}

//Draws particle emitters into the scene.
void Renderer::DrawParticles() {
	SetCurrentShader(particleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), 5.0f);
	UpdateShaderMatrices();
	
	if (weatherSwap) 
		rain->Draw();
	else 
		snow->Draw();

	glUseProgram(0);
}

//TODO: Fix text drawing to screen.
void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	TextMesh* txMesh = new TextMesh(text, *tahoma);

	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix	= camera->BuildViewMatrix();
		projMatrix	= Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		modelMatrix	= Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
	}

	UpdateShaderMatrices();
	txMesh->Draw();
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	delete txMesh;
}

//Moved the attachment checks out of constructor into here.
bool Renderer::LoadCheck() {
	//Individual tests for debugging purposes.
	if (!heightMap->GetTexture()) 
		return false;
	if (!heightMap->GetBumpMap())
		return false;
	if (!cylinder->GetTexture())
		return false;
	if (!cubeMap)
		return false;
	if (!waterQuad->GetTexture())
		return false;
	if (!sceneShader->LinkProgram()) 
		return false;
	if (!celShader->LinkProgram()) 
		return false;
	if (!blurShader->LinkProgram())
		return false;
	if (!reflectShader->LinkProgram())
		return false;
	if (!skyBoxShader->LinkProgram())
		return false;
	if (!shadowShader->LinkProgram())
		return false;
	if (!pointLightShader->LinkProgram())
		return false;
	if (!combineShader->LinkProgram())
		return false;
	if (!simpleShader->LinkProgram())
		return false;
	if (!particleShader->LinkProgram())
		return false;

	return true;
}

//Something something generate screen quad textures.
void Renderer::GenerateScreenTexture(GLuint &into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, 
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, 
		width , height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Rotates the light around the centre of the scene.
void Renderer::RotateLight() {
	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 1000, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	float radius = light->GetRadius();

	Matrix4 lightMatrix = pushMatrix * Matrix4::Rotation(0.5, Vector3(0, 1, 0)) * popMatrix *
							Matrix4::Translation(light->GetPosition()) *
							Matrix4::Scale(Vector3(radius, radius, radius));
	
	light->SetPosition(lightMatrix.GetPositionVector());
	//lightSphere->SetTransform(Matrix4::Translation(lightMatrix.GetPositionVector()));
}

//Returns "ON" or "OFF" depending on the boolean passed to it.
string Renderer::IsOn(bool arg) {
	if (arg)
		return "ON";
	else
		return "OFF";
}

int Renderer::GetTreeMemUsage() {
	int out = 0;
	for (unsigned int i = 0; i < sceneNode->GetNumChild(); ++i) {
		out += sceneNode->GetChildAt(i)->GetMemUsage();
	}
	return out;
}

string Renderer::GetWeatherType() {
	if (!weatherSwap)
		return "Snow";
	else
		return "Rain";
}