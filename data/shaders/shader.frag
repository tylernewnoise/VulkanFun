#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragViewVec;
layout(location = 4) in vec3 fragLightVec;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;
layout(binding = 2) uniform sampler2D normalMap;

void main()
{
    vec3 texColor = texture(tex, fragCoord).xyz;

    vec3 N = normalize(texture(normalMap, fragCoord).xyz);
    vec3 L = normalize(fragLightVec);
    vec3 V = normalize(fragViewVec);
    vec3 R = reflect(L, N);

    vec3 ambient = texColor * 0.1;
    vec3 diffuse = max(dot(N, L), 0.0) * texColor;
    vec3 specular = pow(max(dot(R,V), 0.0), 6.0) * vec3(1.35);

    outColor = vec4(ambient + diffuse + specular, 1.0);
}


/*
 * Shader for shading without any textures!
 *
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragViewVec;
layout(location = 4) in vec3 fragLightVec;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

void main()
{

    vec3 N = normalize(fragNormal);
    vec3 L = normalize(fragLightVec);
    vec3 V = normalize(fragViewVec);
    vec3 R = reflect(L, N);

    vec3 ambient = fragColor * 0.1;
    vec3 diffuse = max(dot(N, L), 0.0) * fragColor;
    vec3 specular = pow(max(dot(R,V), 0.0), 16.0) * vec3(1.35);

    outColor = vec4(ambient + diffuse + specular, 1.0);
}
*/
