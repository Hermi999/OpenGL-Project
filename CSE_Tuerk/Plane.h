#pragma once

#ifndef PLANE_H
#define PLANE_H

#include <GL/glew.h>
#include "SimpleObject.h"

class Plane : public SimpleObject
{
public:

	Plane(GLfloat _vertices[], size_t _sizeof_vertices, GLuint _indices[], size_t _sizeof_indices)
		:SimpleObject(_vertices, _sizeof_vertices, _indices, _sizeof_indices)
	{}
};

#endif

