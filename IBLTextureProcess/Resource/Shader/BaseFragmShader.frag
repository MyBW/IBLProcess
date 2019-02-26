#version 430 core
in vec2 UV;
uniform sampler2D LightMap;
uniform vec3 LineColor;
void main()
{
   vec4 LightMapColor = texture2D(LightMap, UV);
   gl_FragColor = vec4(LineColor, 1.0);
   //gl_FragColor = LightMapColor;
}