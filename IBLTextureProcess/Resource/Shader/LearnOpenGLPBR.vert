
in vec3 inPosition;
in vec3 inNormal;
in vec2 inUV;
in vec3 Tagent;

uniform mat4  ModelMatrix;
uniform mat4  ViewMatrix;
uniform mat4  ProjectMatrix;

out vec2  PsUV; 
out vec3  PsNormal;
out vec3  PsPosition;

void main()
{
   gl_Position = ProjectMatrix * ViewMatrix * ModelMatrix * vec4(inPosition , 1.0) ;
   PsUV = inUV;
   vec4 TmpNoraml = ModelMatrix * vec4(inNormal , 0.0) ;
   vec4 TmpPosition = ModelMatrix * vec4(inPosition , 1.0) ;
   PsNormal = TmpNoraml.xyz;
   PsPosition = TmpPosition.xyz;
}
