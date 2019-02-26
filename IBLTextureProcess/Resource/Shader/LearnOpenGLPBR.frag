
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
   vec3 BaseColor = GetBaseColor();
   vec3 Normal = GetNormal() ;
   vec3 MaterialValue = GetMaterialValue();
   float Metalic = MaterialValue[0];
   float Roughness = MaterialValue[1];
   vec3 SpecularColor = ComputeSpecularColor(BaseColor ,Specular ,Metalic) ;
   vec3 DiffuseColor = ComputeDiffuseColor(BaseColor, Metalic);

   vec3 ViewDirection = normalize(ViewPos - PsPosition.xyz);
   vec3 R = reflect(-ViewDirection , Normal) ;
   float NoV = max(dot(Normal , ViewDirection) , 0.0) ;
   vec3 Lo;
   {
     // calculate per-light radiance
        vec3 L = normalize(LightPos - PsPosition);
        vec3 H = normalize(ViewDirection + L);
        float distance = length(LightPos - PsPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = LightColor * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(Normal, H, Roughness);   
        float G   = GeometrySmith(Normal, ViewDirection, L, Roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, ViewDirection), 0.0), SpecularColor);        
        
        vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(Normal, ViewDirection), 0.0) * max(dot(Normal, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
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
        float NdotL = max(dot(Normal, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * BaseColor / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        //Lo = vec3(NoV);
    }


   vec3 F = fresnelSchlickRoughness(max(dot(Normal , ViewDirection), 0.0) , SpecularColor , Roughness) ;
   vec3 kS = F;
   vec3 kD = 1.0 - kS;
   //IBL: Irradianc
   vec3 irradiance = texture(Irradiance_Light, Normal).rgb;
   vec3 Diffuse      = irradiance * DiffuseColor;

   //IBL: Specluar
   float MipLevel = Roughness * CubemapMaxMip;
   vec3 R = reflect(-ViewDirection, Normal);
   vec3 EvnLighting = textureLod(IBL_Specular_Light, R , MipLevel).rgb ;
   vec2 Brdf = textureLod(IBL_LUT, vec2(NoV , Roughness), 0.0).xy ; 

   vec3 InDirectLightSpecular = EvnLighting * (F * Brdf.x + Brdf.y);
   vec3 InDirectLight = (kD * Diffuse + InDirectLightSpecular);

   vec3 FinalColor = InDirectLight; //InDirectLight + Lo;

   // tonemap
   FinalColor = vec3(1.0) - exp(-FinalColor * Exposure);
   // gamma correct
   FinalColor = pow(FinalColor, vec3(1.0/2.2));

   gl_FragColor.xyz = FinalColor;
   gl_FragColor.a = 1.0;
}
