#pragma once

#ifndef CUBE_H
#define CUBE_H

#include <GL/glew.h>
#include "SimpleObject.h"

class Cube : public SimpleObject
{
public:

	Cube(GLfloat _vertices[], size_t _sizeof_vertices)
		:SimpleObject(_vertices, _sizeof_vertices)
	{}

};

#endif

