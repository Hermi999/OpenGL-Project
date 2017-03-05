#version 330 core

in vec4 vertexColor;
out vec4 color;

void main()
{
	// Linearly interpolate between both textures (second texture is only slightly combined)
	color = vertexColor;
}