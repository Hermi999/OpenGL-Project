#pragma once

#ifndef SIMPLEOBJECT_H
#define SIMPLEOBJECT_H

#include <GL/glew.h>
#include "shader.h"

class SimpleObject
{
public:
	GLfloat* vertices;
	GLuint* indices;
	size_t sizeof_vertices;
	size_t sizeof_indices;
	std::vector<glm::vec3> positions;
	GLfloat color[4];
	GLuint VAO; 
	GLuint VBO;
	GLuint EBO;
	Shader* shader;
	GLuint texture;
	int type;	// 1...vertices, 0...triangles

protected:
	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;

public:
	
	SimpleObject(GLfloat _vertices[], size_t _sizeof_vertices)
	{
		init(_vertices, _sizeof_vertices);
	}
	
	SimpleObject(GLfloat _vertices[], size_t _sizeof_vertices, GLuint _indices[], size_t _sizeof_indices)
	{
		init(_vertices, _sizeof_vertices);

		size_t array_size = _sizeof_indices / sizeof(GLuint);
		indices = new GLuint[array_size];
		memcpy(indices, _indices, _sizeof_indices);
		sizeof_indices = _sizeof_indices;
	}

	~SimpleObject()
	{
		delete vertices;
		delete indices;

		// Properly de-allocate all resources once they've outlived their purpose
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void setColor(GLfloat _color[])
	{
		memcpy(color, _color, 4*sizeof(GLfloat));
	}

	void multiplyObject(glm::vec3 startpos, int count, GLfloat delta)
	{
		int length = pow(count, 1/2.0);
		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				GLfloat x = startpos.x + i*delta;
				GLfloat y = startpos.y;// + j*delta;
				GLfloat z = startpos.z + j*delta;
				positions.push_back(glm::vec3(x, y, z));
			}
		}
	}

	void buildAndCompileShader(char vs[], char frag[])
	{
		shader = new Shader(vs, frag);
	}

	void bindTexture(char name[])
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation((*shader).Program, name), 0);
	}

	void prepare(int _type) {
		type = _type;

		if (type == 0) {
			prepareTriangles();
		}
		else {
			prepareVertices();
		}
	}

	void activateShader(glm::mat4 view, glm::mat4 projection)
	{
		(*shader).Use();

		// Get their uniform location
		modelLoc = glGetUniformLocation((*shader).Program, "model");
		viewLoc = glGetUniformLocation((*shader).Program, "view");
		projLoc = glGetUniformLocation((*shader).Program, "projection");
		// Pass them to the shaders
		/*glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));*/
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	}

	void draw(Camera camera, bool _levelOfDetail) {
		glBindVertexArray(VAO);
		// Calculate model matrix for each object and pass it to shader before drawing
		for (GLuint i = 0; i < positions.size(); i++) {
			glm::mat4 model;
			model = glm::translate(model, positions[i]);

			if (_levelOfDetail == true) {
				levelOfDetail(camera, positions[i]);
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			if (color != NULL) {
				glUniform4fv(glGetUniformLocation((*shader).Program, "inColor"), 1, color);
			}

			if (type == 0) {
				// triangles
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else {
				// vertices
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		glBindVertexArray(0);
	}

	void sortAndDraw(Camera camera, bool _levelOfDetail)
	{
		std::map<GLfloat, glm::vec3> sortedObjects = sortObjects(camera);

		glBindVertexArray(VAO);
		// Calculate model matrix for each object and pass it to shader before drawing
		glm::mat4 model;
		for (std::map<float, glm::vec3>::reverse_iterator it = sortedObjects.rbegin(); it != sortedObjects.rend(); ++it)
		{
			model = glm::mat4();
			model = glm::translate(model, it->second);
			
			if (_levelOfDetail == true) {
				levelOfDetail(camera, it->second);
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			
			if (color != NULL) {
				glUniform4fv(glGetUniformLocation((*shader).Program, "inColor"), 1, color);
			}

			if (type == 0) {
				// triangles
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else {
				// vertices
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
		glBindVertexArray(0);
	}

protected:
	std::map<GLfloat, glm::vec3> sortObjects(Camera camera)
	{
		std::map<GLfloat, glm::vec3> sortedObjects;
		for (GLuint i = 0; i < positions.size(); i++) {
			GLfloat distance = glm::length(camera.Position - positions[i]);
			sortedObjects[distance] = positions[i];
		}
		return sortedObjects;
	}

private:
	void init(GLfloat _vertices[], size_t _sizeof_vertices)
	{
		size_t array_size = _sizeof_vertices / sizeof(GLfloat);
		vertices = new GLfloat[array_size];
		memcpy(vertices, _vertices, _sizeof_vertices);
		sizeof_vertices = _sizeof_vertices;
	}
	
	void prepareTriangles()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof_vertices, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof_indices, indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	}

	void prepareVertices()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof_vertices, vertices, GL_STATIC_DRAW);
		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// TexCoord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0); // Unbind cubeVAO
	}
	
	void levelOfDetail(Camera camera, glm::vec3 pos) {
		GLfloat lod = glm::length(camera.Position - pos);
		if (lod < 20) {
			GLfloat _color1[] = { 1.0f, 236./255., 179/255. };
			glUniform3fv(glGetUniformLocation((*shader).Program, "colorLOD"), 1, _color1);
		}
		else if (lod < 40) {
			GLfloat _color2[] = { 1.0f, 193./255., 7./255. };
			glUniform3fv(glGetUniformLocation((*shader).Program, "colorLOD"), 1, _color2);
		}
		else {
			GLfloat _color3[] = { 1.0f, 111./255., 0.0f  };
			glUniform3fv(glGetUniformLocation((*shader).Program, "colorLOD"), 1, _color3);
		}
	}
};

#endif SIMPLEOBJECT_H