
in vec2  PsUV; 
in vec3  PsNormal;
in vec3  PsPosition;

#ifdef BaseColor_Texture
uniform sampler2D BaseColorTexture;
#else
uniform vec3 PsBaseColor;
#endif

#ifdef Normal_Texture
uniform sampler2D NormalTexture;
#endif

#ifdef Matrial_Texture
uniform sampler2D MaterialTexture;
uniform float Roughness;
#else

uniform float Metalic;
#endif

uniform samplerCube IBL_Specular_Light;
uniform samplerCube Irradiance_Light;
uniform sampler2D  IBL_LUT;
uniform float CubemapMaxMip;
uniform vec3 ViewPos;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform float Exposure;
uniform float Specular;
const float PI = 3.1415926 ;

struct RoughnessInfor 
{
  float envFresnel; // (1.0 - roughness)^2, used for Fresnel with env map
  float envMipLevel0
  float envMipLevel1;
  float envMipMixFactor;
};

vec3 GetNormal()
{
#ifdef Normal_Texture
  vec3 tangentNormal = texture(NormalTexture, PsUV).xyz * 2.0 - 1.0;

  vec3 Q1  = dFdx(PsPosition);
  vec3 Q2  = dFdy(PsPosition);
  vec2 st1 = dFdx(PsUV);
  vec2 st2 = dFdy(PsUV);

  vec3 N   = normalize(PsNormal);
  vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
  vec3 B  = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangentNormal);
#else
  vec3 Normal = PsNormal;
  return normalize(Normal);
#endif
}

vec3 GetMaterialValue()
{
#ifdef Matrial_Texture
    vec3 MaterialData;
    //MaterialData.bgr = texture2D(MaterialTexture, PsUV).rgb;
    vec3 TempMaterial = texture2D(MaterialTexture, PsUV).rgb;
    MaterialData[0] = TempMaterial[2];
    MaterialData[1] = TempMaterial[1];
#else
    vec3 MaterialData;
    MaterialData[0] = Metalic;
    MaterialData[1] = Roughness;
#endif
    return MaterialData;
}

vec3 ComputeDiffuseColor(vec3 BaseColor , float Metalic)
{
  return BaseColor - BaseColor * Metalic;
}

vec3 ComputeSpecularColor(vec3 BaseColor , float Specular, float Metalic)
{
  return mix(vec3(Specular * 0.08) , BaseColor , Metalic);
}
float _g2l(in float x) 
{
  return (x <= 0.04045) ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}
vec3 gammaToLinear(in vec3 rgb)
{
  return vec3(_g2l(rgb.r), _g2l(rgb.g), _g2l(rgb.b));
}
vec3 GetBaseColor(out float alpha)
{
  #ifdef BaseColor_Texture
  vec4 diffuseGamma = texture2D(BaseColorTexture, PsTextureUV);
  vec3 BaseColor = gammaToLinear(diffuseGamma.rgb);
  alpha = diffuseGamma.a;
  #else
  vec3 BaseColor = PsBaseColor;
  #endif
  return clamp(BaseColor,vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
}
vec3 HDRDecode(in vec4 rgbm)
{
  const float RGBMScale = 2.82842712;
  vec3 r = rgbm.rgb * (RGBMScale *(1.0 - rgbm.a));
  r = r * r;
  return r;
}
vec3 getSpecularColor(in vec3 diffuseColor, float metalic) 
{
  const float dielectricF0 = 0.04;
  return mix(vec3(dielectricF0), diffuseColor, metalic);
}
float getLuminance(in vec3 color) 
{
  return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}
RoughnessInfor getRoughnessFactors(float Roughness) 
{
  RoughnessInfor r;
  float Roughness2 = Roughness * Roughness;
  float maxMipLevel = 8.0 - 1.0;
  float mipSelector = Roughness * maxMipLevel;
  r.envMipLevel0 = floor(mipSelector);
  r.envMipLevel1 = min(r.envMipLevel0 + 1.0, maxMipLevel);
  r.envMipMixFactor = fract(mipSelector);
  r.envFresnel = (Roughness2 - 2.0 * Roughness + 1.0);
  return r;
}
vec3 fixSeams(vec3 vec, float mipLevel) 
{
  float scale = 1.0 - exp2(mipLevel) / 128.0;
  float M = max(max(abs(vec.x), abs(vec.y)), abs(vec.z));
  if (abs(vec.x) != M) vec.x *= scale;
  if (abs(vec.y) != M) vec.y *= scale;
  if (abs(vec.z) != M) vec.z *= scale;
  return vec;
}
vec3 getEnvColor(in samplerCube envMap, in RoughnessInfor roughness,in vec3 reflectionW) 
{
  vec3 reflectionNormW = normalize(reflectionW);
  vec3 reflectionNormFixedW = fixSeams(reflectionNormW, roughness.envMipLevel0);
  vec4 cubeColorRaw0 = textureCubeLod(envMap, reflectionNormFixedW, roughness.envMipLevel0);
  vec3 cubeColor0 = HDRDecode(cubeColorRaw0);
  vec4 cubeColorRaw1 = textureCubeLod(envMap, reflectionNormFixedW, roughness.envMipLevel1);
  vec3 cubeColor1 = HDRDecode(cubeColorRaw1);
  return mix(cubeColor0, cubeColor1, roughness.envMipMixFactor);
}
vec3 fresnelSchlickEnv(in vec3 specularColor, in RoughnessInfor roughness, in float clampCosnv, inout float opacity) 
{
  float fresnel = 1.0 - clampCosnv;
  float fresnel2 = fresnel * fresnel;
  fresnel *= fresnel2 * fresnel2;
  fresnel *= roughness.envFresnel;
  opacity = opacity + (1.0 - opacity) * fresnel;
  return specularColor + (1.0 - specularColor) * fresnel;
}
vec3 GetSpecular(in vec3 Diffuse, in vec3 TotalIntensity, inout float Opacity, vec3 ViewDirection, vec3 Normal, vec3 RefectionDirection, float Metallic, float Roughness)
{
  vec3 totalSpec = vec3(0.0, 0.0, 0.0);
  vec3 diffuseIntensity = TotalIntensity * (1.0 - mix(0.04, 1.0, Metallic));
  float clampCosnv = clamp( dot(Normal, ViewDirection), 0.0, 1.0 );
  vec3 specularColor = getSpecularColor(Diffuse, Metallic);
  RoughnessInfor roughness = getRoughnessFactors(Roughness);
  vec3 envColor = getEnvColor(IBLTexture, roughness, RefectionDirection);
  vec3 fresnel = fresnelSchlickEnv(specularColor, roughness, clampCosnv, Opacity);
  totalSpec = envColor * fresnel;
  float luminance = getLuminance(TotalIntensity);
  return Diffuse * TotalIntensity;
  return (Diffuse * diffuseIntensity + totalSpec * mix(0.4, 1.0, luminance));
}
void main()
{
   float Alpha;
   vec3 BaseColor = GetBaseColor(Alpha);
   vec3 Normal = GetNormal() ;
   vec3 MaterialValue = GetMaterialValue();
   float Metalic = MaterialValue[0];
   float Roughness = MaterialValue[1];
   vec3 SpecularColor = ComputeSpecularColor(BaseColor ,Specular ,Metalic) ;
   vec3 DiffuseColor = ComputeDiffuseColor(BaseColor, Metalic);

   vec3 ViewDirection = normalize(ViewPos - PsPosition.xyz);
   vec3 R = reflect(-ViewDirection , Normal) ;
   float NoV = max(dot(Normal , ViewDirection) , 0.0) ;

   vec4 LightmapColor = texture2D(LightmapTexture, PsLightmapUV.xy);
   vec3 LightmapData = HDRDecode(LightmapColor);

   vec3 DiffuseSpecular = GetSpecular(BaseColor, LightmapData, Alpha, ViewDirection, Normal, R, Metalic, Roughness);
   vec3 exposedColor = DiffuseSpecular.xyz;
   vec3 Color = exposedColor;
   Color = Color / (Color + 0.187) * 1.035;
   gl_FragColor = vec4(Color.rgb, Alpha);
}
