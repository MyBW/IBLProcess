#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 ininUV;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
out vec3 WorldPosition ;
void main()
{
	WorldPosition = inPosition;

	gl_Position = ProjectMatrix * ViewMatrix *vec4(inPosition, 1.0) ;

}