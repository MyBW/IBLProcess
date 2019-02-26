#version 430 core

in vec3 inPosition;
in vec2 inUV;
uniform mat4  Orthographic;

out vec2  textureCoord; 
void main()
{
   gl_Position = Orthographic * vec4(inPosition , 1.0) ;
   textureCoord = inUV;
}

