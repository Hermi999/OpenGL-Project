#version 330 core
in vec3 ourColor;

out vec4 color;

void main()
{
	// Linearly interpolate between both textures (second texture is only slightly combined)
	color = vec4(ourColor, 1.0f);
}