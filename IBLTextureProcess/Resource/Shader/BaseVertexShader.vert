#version 430 core

in vec3 Position;
in vec3 Normal;
in vec2 inUV;
in vec3 Tagent;

uniform mat4  ModelMatrix;
uniform mat4  ViewMatrix;
uniform mat4  ProjectMatrix;

out vec2  UV; 
void main()
{
   gl_Position = ProjectMatrix * ViewMatrix * ModelMatrix * vec4(Position , 1.0) ;
   UV = inUV;
}

