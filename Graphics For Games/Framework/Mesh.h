#pragma once
#include "OGLRenderer.h"

#define M_PI 3.14159265358979323846 /* pi */

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, 
	NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MAX_BUFFER 
};

class Mesh {
public:
	Mesh(void);
	~Mesh(void);

	virtual void	Draw(void);
	static Mesh*	GenerateTriangle(void);
	static Mesh*	GenerateQuad(void);
	static Mesh*	GenerateCylinder(int arg_height, int sides);
	static Mesh*	GenerateSphere(float size, int rings, int sectors);
	static Mesh*	GenerateLeaf();

	void			SetTexture(GLuint tex)	{ texture = tex; }
	GLuint			GetTexture(void)		{ return texture; }

	void			SetBumpMap(GLuint tex)	{ bumpTexture = tex; }
	GLuint			GetBumpMap()			{ return bumpTexture; }

protected:
	void			BufferData(void);
	void			GenerateNormals(void);

	void			GenerateTangents();
	Vector3			GenerateTangent(const Vector3 &a,  const Vector3 &b,  const Vector3 &c,
									const Vector2 &ta, const Vector2 &tb, const Vector2 &tc);

	GLuint			texture;
	GLuint			arrayObject;
	GLuint			bufferObject[MAX_BUFFER];
	GLuint			numVertices;
	GLuint			type;
	GLuint			numIndices;
	GLuint			bumpTexture;

	Vector2*		textureCoords;
	Vector3*		vertices;
	Vector3*		normals;
	Vector3*		tangents;
	Vector4*		colours;
	
	unsigned int*	indices;
};