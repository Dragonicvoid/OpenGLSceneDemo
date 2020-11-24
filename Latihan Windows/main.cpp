#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

const float toRadians = 3.14159265f / 180.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

Window mainWindow;
std::vector<Mesh*> meshList;

std::vector<Shader> shaderList;

Texture brickTexture;
Texture steelTexture;

// Vertex Shader
static const char* vShader = "Shaders/normal_shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/normal_shader.frag";

// bisa saja masalah di urutan indicesnya, jadi coba coba aja dulu
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int cubeIndices[] =
	{
		0, 1, 2,
		1, 2, 3,
		4, 5, 0,
		5, 0, 1,
		6, 7, 4,
		4, 7, 5,
		2, 3, 6,
		3, 6, 7,
		1, 5, 7,
		1, 7, 3,
		0, 4, 6,
		0, 6, 2
	};

	GLfloat cubeVertices[] =
	{
		//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f,			0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,			1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,			1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,			0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,			0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,			1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,			1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(cubeIndices, 36, cubeVertices, 64, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(cubeVertices, cubeIndices, 64, 36);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(floorVertices, floorIndices, 36, 6);
	meshList.push_back(obj2);
}

void CreateShader()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void RenderScene()
{
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	meshList[0]->RenderMesh();

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	steelTexture.UseTexture();
	meshList[1]->RenderMesh();
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	shaderList[0].Validate();

	RenderScene();
}

int main()
{
	mainWindow = Window(1920, 1080);
	mainWindow.Initialise();

	brickTexture = Texture("Textures/Wall.jpg");
	brickTexture.LoadTextureWithA();
	steelTexture = Texture("Textures/Steel.png");
	steelTexture.LoadTextureWithA();

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	if (!mainWindow.getShouldClose()) 
	{
		glfwPollEvents();
		//mainWindow.swapBuffers();
	}
	
	return 0;
}