#include "Mesh.h"

Mesh::Mesh(void) {
	for(int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}

	glGenVertexArrays(1, &arrayObject);

	type			= GL_TRIANGLES;

	texture			= 0;
	numIndices		= 0;
	numVertices		= 0;
	bumpTexture		= 0;

	tangents		= NULL;
	vertices		= NULL;
	colours			= NULL;
	textureCoords	= NULL;
	normals			= NULL;
	indices			= NULL;
}

Mesh::~Mesh(void) {
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &bumpTexture);

	delete[] tangents;
	delete[] normals;
	delete[] textureCoords;
	delete[] vertices;
	delete[] colours;
	delete[] indices;
}

Mesh* Mesh::GenerateTriangle(void) {
	Mesh*m = new Mesh();
	m->numVertices = 3;

	m->vertices		= new Vector3[m->numVertices];
	m->vertices[0]	= Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1]	= Vector3(-0.5f, -0.5f, 0.0f);
	m->vertices[2]	= Vector3(0.5f, -0.5f, 0.0f);

	m->textureCoords	= new Vector2[m->numVertices];
	m->textureCoords[0]	= Vector2(0.5f, 0.0f);
	m->textureCoords[1]	= Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours		= new Vector4[m->numVertices];
	m->colours[0]	= Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1]	= Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2]	= Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateQuad(void) {
	Mesh* m				= new Mesh();
	m->numVertices		= 4;
	m->type				= GL_TRIANGLE_STRIP;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];
	m->normals			= new Vector3[m->numVertices];
	m->tangents			= new Vector3[m->numVertices];

	m->vertices[0]		= Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1]		= Vector3(-1.0f,  1.0f, 0.0f);
	m->vertices[2]		= Vector3( 1.0f, -1.0f, 0.0f);
	m->vertices[3]		= Vector3( 1.0f,  1.0f, 0.0f);

	m->textureCoords[0]	= Vector2(0.0f, 1.0f);
	m->textureCoords[1]	= Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3]	= Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		m->colours[i]	= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		m->normals[i]	= Vector3(0.0f, 0.0f, -1.0f);
		m->tangents[i]	= Vector3(1.0f, 0.0f, 0.0f);
	}

	m->BufferData();
	return m;
}


Mesh* Mesh::GenerateCylinder(int arg_height, int sides) {
	//arg_height +=1;
	Mesh* m				= new Mesh();
	m->numVertices		= sides * arg_height;
	m->numIndices		= 6 * (sides - 1) * arg_height;
	
	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];
	m->indices			= new GLuint[m->numIndices];


	float heightInc = 1.0f/(float)arg_height;
	float height = -heightInc;
	float deg = 0.0f;
	float mod = 0.0f;

	for (int i = 0; i < arg_height; ++i) {
		height += heightInc;
		for (int j = 0; j < sides; ++j) {
			m->vertices[i * sides + j]			= Vector3(cos(DegToRad(deg)), i, sin(DegToRad(deg)));
			m->textureCoords[i * sides + j]		= Vector2((float)(i + 1),(float)(j + 1));
			m->colours[i * sides + j]			= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			deg += 360 / ((float)sides - 1);
		}
		deg = 0.0f;
		mod += 0.5f;
	}

	int offset = 0;
	
	//Adapted this for loop from the HeightMap constructor.
	for (int i = 0; i < sides - 1; ++i) {
		for (int j = 0; j < arg_height; ++j) {
			int a = (i			* (sides)) + j;
			int b = ((i + 1)	* (sides)) + j;
			int c = ((i + 1)	* (sides)) + (j + 1);
			int d = (i			* (sides)) + (j + 1);

			m->indices[offset++] = c;
			m->indices[offset++] = b;
			m->indices[offset++] = a;

			m->indices[offset++] = a;
			m->indices[offset++] = d;
			m->indices[offset++] = c;
		}
	}
	
	m->GenerateNormals();
	m->GenerateTangents();
	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateSphere(float size, int rings, int sectors) {
	Mesh* m = new Mesh();
	
	const float R = 1.0f / (float)(rings - 1);
	const float S = 1.0f / (float)(sectors - 1);

	m->numVertices = rings * sectors;
	m->numIndices = rings * sectors * 6;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];
	m->normals			= new Vector3[m->numVertices];
	m->indices			= new GLuint[m->numIndices];

	int offset = 0;

	for (int r = 0; r < rings; ++r) {
		for (int s = 0; s < sectors; ++s) {
			const float y = sin(-(M_PI / 2) + M_PI * r * R);
			const float x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			const float z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			m->textureCoords[offset] = Vector2(s*S, r*R);
			m->vertices[offset] = (Vector3(x, y, z) * size);
			m->colours[offset] = Vector4(0.0f, 0.0f, 0.0f, 0.5f);
			m->normals[offset] = Vector3(x, y, z);
			++offset;
		}
	}

	offset = 0;

	for (int r = 0; r < rings -1; ++r) {
		for (int s = 0; s < sectors - 1; ++s) {
			int a = r * sectors;
			int b = (r + 1) * sectors;

			m->indices[offset++] = a + s;
			m->indices[offset++] = b + s;
			m->indices[offset++] = b + (s + 1);

			m->indices[offset++] = a + s;
			m->indices[offset++] = b + (s + 1);
			m->indices[offset++] = a + (s + 1);
		}
	}

	//m->GenerateTangents();
	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateLeaf() {
	Mesh* m				= new Mesh();

	m->numVertices		= 4;
	m->type				= GL_TRIANGLE_STRIP;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];
	m->normals			= new Vector3[m->numVertices];
	m->tangents			= new Vector3[m->numVertices];

	m->vertices[0]		= Vector3(0.0f, -0.5f, 0.0f);
	m->vertices[1]		= Vector3(0.5f,  0.0f, 0.0f);
	m->vertices[2]		= Vector3(-0.5f, 0.0f, 0.0f);
	m->vertices[3]		= Vector3(0.0f,  1.0f, 0.0f);

	m->textureCoords[0]	= Vector2(0.0f, 1.0f);
	m->textureCoords[1]	= Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3]	= Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		m->colours[i]	= Vector4(0.21f, 0.71f, 0.23f, 1.0f);
		m->normals[i]	= Vector3(0.0f, 0.0f, -1.0f);
		m->tangents[i]	= Vector3(1.0f, 0.0f, 0.0f);
	}

	m->BufferData();
	return m;
}

void Mesh::BufferData(void) {
	//Binds the Vertex array of the Mesh.
	glBindVertexArray(arrayObject);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Buffering data created in GenerateTriangle()
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (textureCoords) {
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector2),
					textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if (colours) {
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}

	if (indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	}

	if (normals) {
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	if (tangents) {
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}

	glBindVertexArray(0);
}

void Mesh::Draw(void) {	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (bumpTexture) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bumpTexture);
	}

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER]) {
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(type, 0, numVertices);
	}
	glBindVertexArray (0);
}

void Mesh::GenerateNormals() {
	if (!normals) {
		normals = new Vector3[numVertices];
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i] = Vector3();
	}
	if (indices) { //Generate per-vertex normals.
		for (GLuint i = 0; i < numIndices; i += 3) {
			unsigned int a = indices[i];
			unsigned int b = indices[i + 1];
			unsigned int c = indices[i + 2];

			Vector3 normal = Vector3::Cross((vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

			normals[a]	+= normal;
			normals[b]	+= normal;
			normals[c]	+= normal;
		}
	}
	else { //It's just a list of triangles, so generate face normals.
		for (GLuint i = 0; i < numVertices; i += 3) {
			Vector3 &a	= vertices[i];
			Vector3 &b	= vertices[i + 1];
			Vector3 &c	= vertices[i + 2];

			Vector3 normal = Vector3::Cross(b-a, c-a);

			normals[i]		= normal;
			normals[i + 1]	= normal;
			normals[i + 2]	= normal;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i].Normalise();
	}
}

void Mesh::GenerateTangents() {
	if (!tangents) {
		tangents = new Vector3[numVertices];
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		tangents[i] = Vector3();
	}
	if (indices) {
		for (GLuint i = 0; i < numIndices; i += 3) {
			int a = indices[i];
			int b = indices[i + 1];
			int c = indices[i + 2];

			Vector3 tangent = GenerateTangent(vertices[a], vertices[b], vertices[c],
											textureCoords[a], textureCoords[b], textureCoords[c]);

			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;
		}
	}
	else {
		for (GLuint i = 0; i < numVertices; i += 3) {
			Vector3 tangent = GenerateTangent(vertices[i], vertices[i + 1], vertices[i + 2],
											textureCoords[i], textureCoords[i + 1], textureCoords[i + 2]);

			tangents[i]		+= tangent;
			tangents[i + 1]	+= tangent;
			tangents[i + 2]	+= tangent;
		}
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		tangents[i].Normalise();
	}
}

Vector3 Mesh::GenerateTangent(const Vector3 &a,  const Vector3 &b,  const Vector3 &c,
							  const Vector2 &ta, const Vector2 &tb, const Vector2 &tc) {
	Vector2 coord1	= tb-ta;
	Vector2 coord2	= tc-ta;

	Vector3 vertex1 = b-a;
	Vector3 vertex2 = c-a;

	Vector3 axis	= Vector3(vertex1 * coord2.y - vertex2 * coord1.y);

	float factor	= 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}
