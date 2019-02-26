#version 430 core
in vec3 CubeMapSamplePos ;
uniform samplerCube CubeMap ;
uniform float Roughness;
float ComputeReflectionCaptureMipFromRoughness(float Roughness, float CubemapMaxMip)
{
   float LevelFrom1X1 = 1 - 1.2 * log2(Roughness);
   return CubemapMaxMip - 1 - LevelFrom1X1;
}
void main()
{
    float index = ComputeReflectionCaptureMipFromRoughness(Roughness, 9);
    vec3 color = textureLod(CubeMap, normalize(CubeMapSamplePos), index).rgb;
    color = vec3(1.0) - exp(-color * 1.0);
    
   //Gamma Correct
   color = pow(color, vec3(1.0 / 2.2));
   gl_FragColor = vec4(color, 1.0);
}