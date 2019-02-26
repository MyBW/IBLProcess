#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 inUV;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
out vec3 WorldPosition ;
void main()
{
	WorldPosition = Position;

	gl_Position = ProjectMatrix * ViewMatrix *vec4(Position, 1.0) ;

}