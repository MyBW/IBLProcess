
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

vec3 GetBaseColor()
{
#ifdef BaseColor_Texture
  vec3 BaseColor = pow(texture2D(BaseColorTexture, PsUV).rgb, vec3(2.2));
  //vec3 BaseColor = texture2D(BaseColorTexture, PsUV).rgb;
#else
  vec3 BaseColor = PsBaseColor;
#endif
return BaseColor;
}

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
    //MaterialData[1] = max(Roughness , 0.0) ;
    return MaterialData;
}

void ComputeAlbedoAndSpecular(in vec3 InBaseColor ,in float InMetalic ,in float InSpecular , out vec3 OutAlbedo, out vec3 OutSpecular)
{
    OutAlbedo = InBaseColor *( 1.0 - InMetalic) ;
    float DielectircSpecular = mix(0.0 , 0.08 , InSpecular) ;
    OutSpecular = mix(vec3(DielectircSpecular) , InBaseColor , InMetalic) ;
}

vec3 ComputerNormal(vec2 InNormalXY)
{
   vec3 Normal ;
   float sinThta = sin(InNormalXY.x) ;
   float cosThta = cos(InNormalXY.x) ;
   float sinPh = sin(InNormalXY.y ) ;
   float cosPh = cos(InNormalXY.y ) ;
   Normal.x = sinThta * cosPh ;
   Normal.y = sinThta * sinPh ;
   Normal.z = cosThta ;
   return Normal;
}



vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 




struct FThreeBandSHVector
{
	vec4  V0;
	vec4  V1;
	float  V2;
};


struct FThreeBandSHVectorRGB
{
	FThreeBandSHVector R;
	FThreeBandSHVector G;
	FThreeBandSHVector B;
};



float  DotSH3(FThreeBandSHVector A,FThreeBandSHVector B)
{
	float  Result = dot(A.V0, B.V0);
	Result += dot(A.V1, B.V1);
	Result += A.V2 * B.V2;
	return Result;
}

vec3  DotSH3RGB(FThreeBandSHVectorRGB A,FThreeBandSHVector B)
{
	vec3  Result = vec3(0.0, 0.0, 0.0);
	Result.r = DotSH3(A.R,B);
	Result.g = DotSH3(A.G,B);
	Result.b = DotSH3(A.B,B);
	return Result;
}

#define REFLECTION_CAPTURE_ROUGHEST_MIP 1
#define REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE 1.2

float ComputeReflectionCaptureRoughnessFromMip(float Mip, float CubemapMaxMip)
{
  float LevelFrom1x1 = CubemapMaxMip - 1 - Mip;
  return exp2( ( REFLECTION_CAPTURE_ROUGHEST_MIP - LevelFrom1x1 ) / REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE );
}


float ComputeReflectionCaptureMipFromRoughness(float Roughness, float CubemapMaxMip)
{
   float LevelFrom1X1 = REFLECTION_CAPTURE_ROUGHEST_MIP - REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE * log2(Roughness);
   return CubemapMaxMip - REFLECTION_CAPTURE_ROUGHEST_MIP - LevelFrom1X1;
}


vec3 ComputeDiffuseColor(vec3 BaseColor , float Metalic)
{
  return BaseColor - BaseColor * Metalic;
}

vec3 ComputeSpecularColor(vec3 BaseColor , float Specular, float Metalic)
{
  return mix(vec3(Specular * 0.08) , BaseColor , Metalic);
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
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 EnvBRDF( vec3 SpecularColor, float Roughness, float NoV )
{
  // Importance sampled preintegrated G * F
  vec2 Brdf = textureLod(IBL_LUT, vec2(max(NoV,0.0), Roughness), 0.0).rg;
  // Anything less than 2% is physically impossible and is instead considered to be shadowing 
  vec3 GF = SpecularColor * Brdf.x + max( 5.0 * SpecularColor.g ,0.0) * Brdf.y;
  return GF;
}
// Point lobe in off-specular peak direction
vec3 GetOffSpecularPeakReflectionDir(vec3 Normal, vec3 ReflectionVector, float Roughness)
{
  float a = Roughness * Roughness;
  return mix( Normal, ReflectionVector, (1 - a) * ( sqrt(1 - a) + a ) ); 
}


float NormalCurvatureToRoughness(vec3 WorldNormal)
{
    vec3 NormalCurvatureToRoughnessScaleBias = vec3(1.0 ,0.0, 0.3333);
    vec3 dNdx = dFdx(WorldNormal);
    vec3 dNdy = dFdy(WorldNormal);
    float x = dot(dNdx, dNdx);
    float y = dot(dNdy, dNdy);
    float CurvatureApprox = pow(max(x, y), NormalCurvatureToRoughnessScaleBias.z);
  return max(CurvatureApprox * NormalCurvatureToRoughnessScaleBias.x + NormalCurvatureToRoughnessScaleBias.y , 0.0);
}

void main()
{
   vec3 BaseColor = GetBaseColor();
   vec3 Normal = GetNormal() ;
   vec3 MaterialValue = GetMaterialValue();
   float Metalic = MaterialValue[0];
   float Roughness = max(MaterialValue[1] ,NormalCurvatureToRoughness(Normal));
   vec3 SpecularColor = ComputeSpecularColor(BaseColor ,Specular ,Metalic) ;

   vec3 F0 = vec3(0.04); 
   SpecularColor = mix(F0, BaseColor, Metalic);

   vec3 ViewDirection = normalize(ViewPos - PsPosition.xyz);
   vec3 R = 2.0 * dot(Normal, ViewDirection) * Normal - ViewDirection;
   //R = GetOffSpecularPeakReflectionDir(Normal, R, Roughness);
   float NoV = max(dot(Normal , ViewDirection) , 0.0) ;


   vec3 InF = fresnelSchlickRoughness(max(dot(Normal , ViewDirection), 0.0) , SpecularColor , Roughness) ; 
   
   vec3 kS = InF;
   vec3 kD = 1.0 - kS;
    kD *= 1.0 - Metalic;
   //IBL: Irradianc
   vec3 irradiance = texture(Irradiance_Light, Normal).rgb;
   vec3 diffuse      = irradiance * BaseColor;

   //IBL: Specluar
   float MipLevel = ComputeReflectionCaptureMipFromRoughness(Roughness , CubemapMaxMip);
   vec3 InDirectLightSpecularPart1 = textureLod(IBL_Specular_Light, R , MipLevel).rgb ;
   vec3 InDirectLightSpecularPart2 = EnvBRDF(SpecularColor, Roughness, NoV);
   vec3 InDirectLightSpecular = InDirectLightSpecularPart1 * InDirectLightSpecularPart2 ;


   vec3 InDirectLight = (kD * diffuse + InDirectLightSpecular);
   vec3 FinalColor = InDirectLight; //InDirectLight + Lo;

   // tonemap
   FinalColor = vec3(1.0) - exp(-FinalColor * Exposure);
   // gamma correct
   FinalColor = pow(FinalColor, vec3(1.0/2.2));

   gl_FragColor.xyz = FinalColor;
   gl_FragColor.a = 1.0;
}
