#version 430 core
in vec2 textureCoord;
uniform sampler2D ImageTex;
uniform float Exposure;
void main()
{
   vec3 LightMapColor = texture2D(ImageTex, textureCoord).xyz;
   //LightMapColor = vec3(1.0, 0.0, 0.0);

   //tonemap 1
   //LightMapColor = LightMapColor /(LightMapColor + vec3(1.0));
   //tonemap 2
   //LightMapColor = vec3(1.0) - exp(-LightMapColor * Exposure);

   //Gamma Correct
   //LightMapColor = pow(LightMapColor, vec3(1.0 / 2.2));
   gl_FragColor = vec4(LightMapColor, 1.0);
}