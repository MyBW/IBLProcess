#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 inUV;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform mat4 ModelMatrix;
out vec3 CubeMapSamplePos ;
void main()
{
	CubeMapSamplePos = Position * 2.0 ;
	gl_Position = ProjectMatrix * ViewMatrix * vec4(Position, 0.0) ;
    gl_Position = gl_Position.xyww;
}