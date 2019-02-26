#version 430 core
in vec2  UV; 
in vec3  Normal;
in vec3  Position;

uniform samplerCube IBL_Specular_Light;
uniform samplerCube Irradiance_Light;
uniform sampler2D  IBL_LUT;
uniform float CubemapMaxMip;
uniform vec3 ViewPos;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform sampler2D BaseColorTexture;
uniform float Roughness;
uniform float Specular;
uniform float Metalic;
uniform float Exposure;
const float PI = 3.1415926 ;


vec4 FromScreenToWorld(mat4 InViewInverse , vec2 ClipSpaceXY ,float InFov , float InPrjPlaneWInverseH,float InCameraDepth)
{
   vec2 Pos;
   Pos.xy = ClipSpaceXY;
   vec4 CameraSpacePos;
   CameraSpacePos.x = Pos.x * InPrjPlaneWInverseH * tan(InFov/2.0f) * InCameraDepth;
   CameraSpacePos.y = Pos.y * tan(InFov / 2.0f) * InCameraDepth;
   CameraSpacePos.z = InCameraDepth;
   CameraSpacePos.w = 1.0f;
   return InViewInverse * CameraSpacePos;
}


void ComputeAlbedoAndSpecular(in vec3 InBaseColor ,in float InMetalic ,in float InSpecular , out vec3 OutAlbedo, out vec3 OutSpecular)
{
    OutAlbedo = InBaseColor *( 1 - InMetalic) ;
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



FThreeBandSHVector SHBasisFunction3( vec3  InputVector)
{
	FThreeBandSHVector Result;

	Result.V0.x = 0.282095f;
	Result.V0.y = -0.488603f * InputVector.y;
	Result.V0.z = 0.488603f * InputVector.z;
	Result.V0.w = -0.488603f * InputVector.x;

	vec3  VectorSquared = InputVector * InputVector;
	Result.V1.x = 1.092548f * InputVector.x * InputVector.y;
	Result.V1.y = -1.092548f * InputVector.y * InputVector.z;
	Result.V1.z = 0.315392f * (3.0f * VectorSquared.z - 1.0f);
	Result.V1.w = -1.092548f * InputVector.x * InputVector.z;
	Result.V2 = 0.546274f * (VectorSquared.x - VectorSquared.y);

	return Result;
}
FThreeBandSHVector CalcDiffuseTransferSH3( vec3 Normal, float  Exponent)
{
	FThreeBandSHVector Result = SHBasisFunction3(Normal);



	float  L0 = 2 * PI / (1 + 1 * Exponent );
	float  L1 = 2 * PI / (2 + 1 * Exponent );
	float  L2 = Exponent * 2 * PI / (3 + 4 * Exponent + Exponent * Exponent );
	float  L3 = (Exponent - 1) * 2 * PI / (8 + 6 * Exponent + Exponent * Exponent );


	Result.V0.x *= L0;
	Result.V0.yzw *= L1;
	Result.V1.xyzw *= L2;
	Result.V2 *= L2;

	return Result;
}



float ComputeReflectionCaptureMipFromRoughness(float Roughness, float CubemapMaxMip)
{
   float LevelFrom1X1 = 1 - 1.2 * log2(Roughness);
   return CubemapMaxMip - 1 - LevelFrom1X1;
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
void main()
{
   vec3 BaseColor = pow(texture2D(BaseColorTexture, UV).rgb, vec3(2.2));
   vec3 WorldNormal = normalize(Normal) ;
   vec3 DiffuseColor = ComputeDiffuseColor(BaseColor, Metalic);
   vec3 SpecularColor = ComputeSpecularColor(BaseColor ,Specular ,Metalic) ;
  

   vec3 ViewDirection = normalize(ViewPos - Position.xyz);
   vec3 R = reflect(-ViewDirection , WorldNormal) ;
   float NoV = max(dot(WorldNormal , ViewDirection) , 0.0) ;
   vec3 Lo;
   {
     // calculate per-light radiance
        vec3 L = normalize(LightPos - Position);
        vec3 H = normalize(ViewDirection + L);
        float distance = length(LightPos - Position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = LightColor * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(WorldNormal, H, Roughness);   
        float G   = GeometrySmith(WorldNormal, ViewDirection, L, Roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, ViewDirection), 0.0), SpecularColor);        
        
        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(WorldNormal, ViewDirection), 0.0) * max(dot(WorldNormal, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - Metalic;                 
            
        // scale light by NdotL
        float NdotL = max(dot(WorldNormal, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * BaseColor / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        //Lo = vec3(NdotL);
    }


   vec3 InF = fresnelSchlickRoughness(max(dot(WorldNormal , ViewDirection), 0.0) , SpecularColor , Roughness) ; 
   
   vec3 kS = InF;
   vec3 kD = 1.0 - kS;
    kD *= 1.0 - Metalic;
   //IBL: Irradianc
   vec3 irradiance = texture(Irradiance_Light, WorldNormal).rgb;
   vec3 diffuse      = irradiance * BaseColor;

   //IBL: Specluar
   //float MipLevel = ComputeReflectionCaptureMipFromRoughness(Roughness , CubemapMaxMip);
   float MipLevel = Roughness * CubemapMaxMip;
   vec3 InDirectLightSpecularPart1 = textureLod(IBL_Specular_Light, R , MipLevel).rgb ;

   vec2 Brdf = texture(IBL_LUT, vec2(NoV , Roughness)).xy ; 
   vec3 InDirectLightSpecularPart2 = InF * Brdf.x + Brdf.y ;

   vec3 InDirectLightSpecular = InDirectLightSpecularPart1 * InDirectLightSpecularPart2 ;
   vec3 InDirectLight = (kD * diffuse + InDirectLightSpecular);

   vec3 FinalColor = InDirectLight + Lo;

   // tonemap
   FinalColor = vec3(1.0) - exp(-FinalColor * Exposure);
   // gamma correct
   FinalColor = pow(FinalColor, vec3(1.0/2.2));

   gl_FragColor.xyz = FinalColor;
   gl_FragColor.a = 1.0;
}