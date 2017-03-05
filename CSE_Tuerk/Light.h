#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>
#include "SimpleObject.h"

class Light : public SimpleObject
{
public:

	Light(GLfloat _vertices[], size_t _sizeof_vertices)
		:SimpleObject(_vertices, _sizeof_vertices)
	{}

protected:
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
		glBindVertexArray(0); // Unbind cubeVAO
	}
};

#endif

#pragma once
