#include "Plant.h"

Mesh* Plant::cylinder	= NULL;
Mesh* Plant::leaf		= NULL;
Mesh* Plant::sphere		= NULL;

Plant::Plant(float iter) {
	if (!cylinder)
		CreateBranch();
	if (!leaf)
		CreateLeaf();
	if (!sphere)
		CreateSphere();

	branchLength = iter;

	growRate = 0.07f;
	branches = 3;

	memUsage = sizeof(*leaf) + sizeof(*cylinder) + sizeof(*sphere);

	trunk = new SceneNode();
	trunk->SetMesh(cylinder);
	trunk->SetModelScale(Vector3(0,0,0));
	trunk->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	memUsage += sizeof(*trunk);
	AddChild(trunk);

	CreateTree(trunk, branchLength);
}

void Plant::CreateTree(SceneNode* arg, float branchLen) {
	float outBranchLen = branchLen - 1.0f;
	if (outBranchLen >= 1.0f) 
	{
		for (unsigned int a = 0; a < branches; ++a) 
		{
			SceneNode* branch = new SceneNode();
			branch->SetMesh(cylinder);
			branch->SetModelScale(Vector3(0,0,0));
			arg->AddChild(branch);
			arg->GetChildAt(a)->SetTransform(Matrix4::Translation(Vector3(0, GetHeight(branchLen, 14.0f), 0)) *
											(Matrix4::Rotation(((a * (360/branches)) + (rand() % (360/branches))), Vector3(0,1,0)) *
											 Matrix4::Rotation(40, Vector3(0,0,1))));
			memUsage += sizeof(*branch);
			CreateTree(arg->GetChildAt(a), outBranchLen);
		}
	}
	else
	{
		for (int i = 0; i < ((rand() % 5 + 1) * 2); ++i) {
			if (rand() % 30 == 0) {
				SceneNode* fruit = new SceneNode();
				fruit->SetMesh(sphere);
				fruit->SetModelScale(Vector3(0,0,0));
				fruit->SetTransform(Matrix4::Translation(Vector3(0, (GetHeight(branchLen, 14.0f) / (rand() % 4 + 1)), 0)));
				memUsage += sizeof(*fruit);
				arg->AddChild(fruit);
			} 
			else {
				SceneNode* leafNode = new SceneNode();
				leafNode->SetMesh(leaf);
				leafNode->SetModelScale(Vector3(0,0,0));
				leafNode->SetTransform(Matrix4::Translation(Vector3(0, (GetHeight(branchLen, 14.0f)/i + 1), 0)) *
								   Matrix4::Rotation(90, Vector3(1,1,0)) *
								   Matrix4::Rotation(rand() % 360, Vector3(1,0,0)) *
								   Matrix4::Translation(Vector3(0, 5, 0)) );
				memUsage += sizeof(*leafNode);
				arg->AddChild(leafNode);
			}
		}
	}
}

void Plant::CreateBranch() {
	cylinder = Mesh::GenerateCylinder(100, 10);
	cylinder->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"bark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	glBindTexture(GL_TEXTURE_2D, cylinder->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Plant::CreateLeaf() {
	leaf = Mesh::GenerateLeaf();
	leaf->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"leaf.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
}

void Plant::CreateSphere() {
	sphere = Mesh::GenerateSphere(0.5, 12, 24);
	sphere->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"fruit.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	glBindTexture(GL_TEXTURE_2D, sphere->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Plant::Update(float msec) {
	ApplyTransform(this, branchLength);

	SceneNode::Update(msec);
}

float Plant::GetHeight(float arg, float mod) {
	float out;

	out = 2 * (arg / growRate);
	out -= mod*(out/100);

	return out;
}

void Plant::ApplyTransform(SceneNode* arg, float branchLen) {
	float outBranchLen = branchLen - 1.0f;
	if (arg->GetNumChild() > 0) 
	{
		for (unsigned int i = 0; i < arg->GetNumChild(); ++i) 
		{
			if ((branchLen < 1.0f) && (arg->GetChildAt(i)->GetModelScale().x < 11.0f))
			{
				arg->GetChildAt(i)->SetModelScale(arg->GetChildAt(i)->GetModelScale() + Vector3(growRate * 2,growRate * 2,growRate * 2));
			}
			if (arg->GetChildAt(i)->GetModelScale().x < branchLen) 
			{
				arg->GetChildAt(i)->SetModelScale(arg->GetChildAt(i)->GetModelScale() + Vector3(growRate,0.1,growRate));
			}
			else
			{
				ApplyTransform(arg->GetChildAt(i), outBranchLen);
			}
		}
	}
}