#version 430 core
out vec4 FragColor;
in vec3 WorldPosition;

uniform samplerCube SrcCubemap;

const float PI = 3.14159265359;
vec4 HDREncode(in vec3 rgb) {
  const float rgbmScale = 2.82842712;  // sqrt(8)
  vec3 r = sqrt(rgb) / rgbmScale;
  float m = max(max(r.r, r.g), r.b);
  m = clamp(m, 1.0 / 255.0, 1.0);
  m = ceil(m * 255.0) / 255.0;
  r /= m;
  return vec4(r.r, r.g, r.b, (1.0 - m));
}
void main()
{		
    vec3 N = normalize(WorldPosition);

    vec3 irradiance = vec3(0.0);   
    
    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up            = cross(N, right);
       
    float sampleDelta = 0.025;
    float nrSamples = 0.0f;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += texture(SrcCubemap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    vec4 FinalColor = HDREncode(irradiance);
    FragColor = FinalColor;
}



