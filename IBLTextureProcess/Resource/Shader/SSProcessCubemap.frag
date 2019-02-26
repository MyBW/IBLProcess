#version 430 core
float PI = 3.1415926;

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
mat3 matrixFromVector(vec3 n) { // frisvad
  float a = 1.0 / (1.0 + n.z);
  float b = -n.x * n.y * a;
  vec3 b1 = vec3(1.0 - n.x * n.x * a, b, -n.x);
  vec3 b2 = vec3(b, 1.0 - n.y * n.y * a, -n.y);
  return mat3(b1, b2, n);
}
float rnd(vec2 uv) {
  return fract(sin(dot(uv, vec2(12.9898, 78.233) * 2.0)) * 43758.5453);
}
// cos + lerped cone size (better than just lerped)
vec3 hemisphereSample_cos(vec2 uv, mat3 vecSpace, vec3 cubeDir, float gloss) {
  float phi = uv.y * 2.0 * PI;
  float cosTheta = sqrt(1.0 - uv.x);
  float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
  vec3 sampleDir = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
  return normalize(mix(vecSpace * sampleDir, cubeDir, gloss));
}

in vec3 WorldPosition ;
uniform float Roughness ;
uniform samplerCube CubeMap ;
void main()
{
  vec3 TmpWorldPos = WorldPosition ;
  vec3 Normal = normalize(TmpWorldPos) ;
  mat3 vecSpace = matrixFromVector(normalize(Normal));
  vec3 color = vec3(0.0);
  const int samples = 200;
  vec3 vect;
  for(int i = 0; i < samples; i++) {
    float sini = sin(float(i));
    float cosi = cos(float(i));
    float rand = rnd(vec2(sini, cosi));

    vect = hemisphereSample_cos(
        vec2(float(i) / float(samples), rand), vecSpace, Normal, Roughness);

    color += textureLod(CubeMap, vect, 0.0).rgb;
  }
  color /= float(samples);

  gl_FragColor = HDREncode(color);
}