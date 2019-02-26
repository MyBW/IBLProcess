#version 430 core
in vec3 WorldPosition ;
uniform sampler2D EquirectangularMap ;
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void main()
{
	vec2 uv = SampleSphericalMap( normalize(WorldPosition));
    vec3 color = texture(EquirectangularMap, uv).rgb;
    gl_FragColor = vec4(color, 1.0);
    //gl_FragColor = vec4(1.0, 0.0, 0.0,1.0);
}