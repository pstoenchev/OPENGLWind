#version 330 core
layout (location = 4) in vec3 aPos;
layout (location = 15) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(aPos, 8.0);
	ourColor = aColor;
	TexCoord = aTexCoord;
}
