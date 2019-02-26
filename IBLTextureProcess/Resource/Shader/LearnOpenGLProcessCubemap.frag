#version 430 core
/*float RadicalInverse(int Base , int i)
{
   float Digit , Radical , Inverse ;
   float DBase = Base ;
   Digit = Radical = 1.0/ DBase ;
   Inverse = 0.0 ;
   while(i != 0)
   {
      Inverse += Digit * (i % Base) ;
    Digit *= Radical ;
    i /= Base ;
   }
   return Inverse ;
}

float Hammersley(int Dimension , int Index , int NumberSamples)
{
    if(Dimension == 0)
  {
     float TmpNumberSamples = NumberSamples ;
     return  Index / TmpNumberSamples ;
  }
  else
  {
     return RadicalInverse(2 , Index) ;
  }
}*/
float PI = 3.1415926;
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
  float a = roughness*roughness;
  float phi = 2.0 * PI * Xi.x;
  float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
  float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
  
  // from spherical coordinates to cartesian coordinates - halfway vector
  vec3 H;
  H.x = cos(phi) * sinTheta;
  H.y = sin(phi) * sinTheta;
  H.z = cosTheta;
  
  // from tangent-space H vector to world-space sample vector
  vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent   = normalize(cross(up, N));
  vec3 bitangent = cross(N, tangent);
  
  vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
  return normalize(sampleVec);
}

//HDREncode  and HDRDecode will transform HDR to Unsigned byte
vec4 HDREncode(in vec3 rgb) {
  const float rgbmScale = 2.82842712;  // sqrt(8)
  vec3 r = sqrt(rgb) / rgbmScale;
  float m = max(max(r.r, r.g), r.b);
  m = clamp(m, 1.0 / 255.0, 1.0);
  m = ceil(m * 255.0) / 255.0;
  r /= m;
  return vec4(r.r, r.g, r.b, (1.0 - m));
}
vec3 HDRDecode(in vec4 rgbm) 
{
  const float rgbmScale = 2.82842712;  // sqrt(8)
  vec3 r = rgbm.rgb * (rgbmScale * (1.0 - rgbm.a));
  return r * r;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
in vec3 WorldPosition ;
uniform float Roughness ;
uniform samplerCube CubeMap ;
void main()
{
    vec3 TmpWorldPos = WorldPosition ;
    vec3 Normal = normalize(TmpWorldPos) ;
    vec3 R = Normal ;
    vec3 V = R ;

    int SampleNumber  = 4096 ;
    vec3 Sum = vec3(0.0) ;
    float SumWidght = 0.0 ;
    for (int i = 0; i < SampleNumber; ++i)
    {
        vec2 Xi = Hammersley(i , SampleNumber) ;
        vec3 H = ImportanceSampleGGX(Xi , Normal , Roughness) ;
        vec3 L = normalize(2.0*dot(H , V) * H - V) ;

        float NoL = dot(Normal , L) ;
        if (NoL > 0.0f)
        {
            // sample from the environment's mip level based on roughness/pdf
            float D   = DistributionGGX(Normal, H, Roughness);
            float NdotH = max(dot(Normal, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float resolution = 512.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SampleNumber) * pdf + 0.0001);

            float mipLevel = Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 

            Sum += textureLod(CubeMap , L, mipLevel).rgb * NoL ;
            SumWidght += NoL ;
        }
    }
    vec4 FinalColor = HDREncode(Sum/SumWidght);
    //vec4 FinalColor =  vec4(Sum/SumWidght, 1.0) ;
    gl_FragColor = FinalColor;
    //FinalColor = FinalColor / (FinalColor + 0.187) * 1.035;
    //gl_FragColor = FinalColor;
    //gl_FragColor = vec4(1.0 ,0.0 ,0.0 ,1.0) ;
    //gl_FragColor = vec4(Sum / SumWidght ,1.0) ;
}


