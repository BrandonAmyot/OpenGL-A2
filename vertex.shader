#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 newColour;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main()
{
	gl_Position = /*proj_matrix * view_matrix * model_matrix * */vec4(0.0f, 0.0f, 0.0f, 1.0f);
	newColour = color;
}