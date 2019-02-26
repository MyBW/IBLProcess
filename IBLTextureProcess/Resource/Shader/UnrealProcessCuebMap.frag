#version 430 core


#define REFLECTION_CAPTURE_ROUGHEST_MIP 1
#define REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE 1.2
float PI = 3.1415926;

int ReverseBits32( int bits )
{

  bits = ( bits << 16) | ( bits >> 16);
  bits = ( (bits & 0x00ff00ff) << 8 ) | ( (bits & 0xff00ff00) >> 8 );
  bits = ( (bits & 0x0f0f0f0f) << 4 ) | ( (bits & 0xf0f0f0f0) >> 4 );
  bits = ( (bits & 0x33333333) << 2 ) | ( (bits & 0xcccccccc) >> 2 );
  bits = ( (bits & 0x55555555) << 1 ) | ( (bits & 0xaaaaaaaa) >> 1 );
  return bits;
}

vec2 Hammersley( int Index, int NumSamples, ivec2 Random )
{
  float fIndex = Index;
  float E1 = fract( fIndex / NumSamples + float( Random.x & 0xffff ) / (1<<16) );
  float E2 = float( ReverseBits32(Index) ^ Random.y ) * 2.3283064365386963e-10;
  return vec2( E1, E2 );
}

float D_GGX( float a2, float NoH )
{
  float d = ( NoH * a2 - NoH ) * NoH + 1; // 2 mad
  return a2 / ( PI*d*d );         // 4 mul, 1 rcp
}

vec4 ImportanceSampleGGX( vec2 E, float a2 )
{
  float Phi = 2 * PI * E.x;
  float CosTheta = sqrt( (1 - E.y) / ( 1 + (a2 - 1) * E.y ) );
  float SinTheta = sqrt( 1 - CosTheta * CosTheta );

  vec3 H;
  H.x = SinTheta * cos( Phi );
  H.y = SinTheta * sin( Phi );
  H.z = CosTheta;
  
  float d = ( CosTheta * a2 - CosTheta ) * CosTheta + 1;
  float D = a2 / ( PI*d*d );
  float PDF = D * CosTheta;

  return vec4( H, PDF );
}

vec4 CosineSampleHemisphere( vec2 E )
{
  float Phi = 2 * PI * E.x;
  float CosTheta = sqrt( E.y );
  float SinTheta = sqrt( 1 - CosTheta * CosTheta );

  vec3 H;
  H.x = SinTheta * cos( Phi );
  H.y = SinTheta * sin( Phi );
  H.z = CosTheta;

  float PDF = CosTheta * (1 / PI);

  return vec4( H, PDF );
}


mat3x3 GetTangentBasis( vec3 TangentZ )
{
  vec3 UpVector = abs(TangentZ.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
  vec3 TangentX = normalize( cross( UpVector, TangentZ ) );
  vec3 TangentY = cross( TangentZ, TangentX );
  return mat3x3( TangentX, TangentY, TangentZ );
}

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


in vec3 WorldPosition ;
uniform int NumMips;
uniform int MipIndex;
uniform samplerCube CubeMap ;

void main()
{
    vec3 TmpWorldPos = WorldPosition ;
    vec3 Normal = normalize(TmpWorldPos) ;
    vec3 R = Normal ;
    vec3 V = R ;
    mat3x3 TangentToWorld = GetTangentBasis(Normal);
     
    float Roughness = ComputeReflectionCaptureRoughnessFromMip( MipIndex, NumMips);

    if( Roughness < 0.01 )
    {
      gl_FragColor = vec4(textureLod(CubeMap , Normal, 0.0).rgb , 1.0);
      return;
    }
    int CubeSize = 1 << ( NumMips - 1 );
    const float SolidAngleTexel = 4*PI / ( 6 * CubeSize * CubeSize ) * 2;
    const int NumSamples = Roughness < 0.1 ? 32 : 64;
    ivec2 Random = ivec2(0.0, 0.0);
    vec4 FilteredColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 FinalColor;
    if( Roughness > 0.99 )
    {
      // Roughness=1, GGX is constant. Use cosine distribution instead
      for( int i = 0; i < NumSamples; i++ )
      {
        vec2 E = Hammersley( i, NumSamples, Random );

        vec3 L = CosineSampleHemisphere( E ).xyz;

        float NoL = L.z;

        float PDF = NoL / PI;
        float SolidAngleSample = 1.0 / ( NumSamples * PDF );
        float Mip = 0.5 * log2( SolidAngleSample / SolidAngleTexel );

        L = TangentToWorld * L;
        FilteredColor += textureLod(CubeMap , Normal, Mip);
      }

      FinalColor = FilteredColor / NumSamples;
    }
    else
    {
        float Weight = 0;

      for( int i = 0; i < NumSamples; i++ )
      {
        vec2 E = Hammersley( i, NumSamples, Random );

        // 6x6 Offset rows. Forms uniform star pattern
        //uint2 Index = uint2( i % 6, i / 6 );
        //float2 E = ( Index + 0.5 ) / 5.8;
        //E.x = frac( E.x + (Index.y & 1) * (0.5 / 6.0) );

        E.y *= 0.995;

        vec3 H = ImportanceSampleGGX( E, pow(Roughness,4) ).xyz;
        vec3 L = 2 * H.z * H - vec3(0,0,1);

        float NoL = L.z;
        float NoH = H.z;

        if( NoL > 0 )
        {
          //float TexelWeight = CubeTexelWeight( L );
          //float SolidAngleTexel = SolidAngleAvgTexel * TexelWeight;

          //float PDF = D_GGX( pow(Roughness,4), NoH ) * NoH / (4 * VoH);
          float PDF = D_GGX( pow(Roughness,4), NoH ) * 0.25;
          float SolidAngleSample = 1.0 / ( NumSamples * PDF );
          float Mip = 0.5 * log2( SolidAngleSample / SolidAngleTexel );

          float ConeAngle = acos( 1 - SolidAngleSample / (2*PI) );

          L = TangentToWorld * L;
          FilteredColor += textureLod(CubeMap , Normal, Mip) * NoL;
          Weight += NoL;
        }
      }

      FinalColor = FilteredColor / Weight;
    }

    //vec4 FinalColor = HDREncode(Sum/SumWidght);
    gl_FragColor = FinalColor;
    //FinalColor = FinalColor / (FinalColor + 0.187) * 1.035;
    //gl_FragColor = FinalColor;
    //gl_FragColor = vec4(1.0 ,0.0 ,0.0 ,1.0) ;
    //gl_FragColor = vec4(Sum / SumWidght ,1.0) ;
}


