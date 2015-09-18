#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, MAX_BUFFER 
};

class Mesh {
public:
	Mesh(void);
	~Mesh(void);

	virtual void	Draw(void);
	static Mesh*	GenerateTriangle(void);
	static Mesh*	GenerateQuad(void);
	
	void			SetTexture(GLuint tex)	{ texture = tex; }
	void			SetTexture2(GLuint tex)	{ texture2 = tex; }
	GLuint			GetTexture(void)		{ return texture; }

protected:
	void		BufferData(void);

	GLuint		texture, texture2;
	Vector2*	textureCoords;

	GLuint		arrayObject;
	GLuint		bufferObject[MAX_BUFFER];
	GLuint		numVertices;
	GLuint		type;

	Vector3*	vertices;
	Vector4*	colours;
};