#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragViewVec;
layout(location = 4) in vec3 fragLightVec;
layout(binding = 1) uniform sampler2D texSampler;

// which framebuffer to put the color
layout(location = 0) out vec4 outColor;

void main()
{
    // outColor = vec4(fragTexCoord, 0.0, 1.0); //RGBAlpha, colors only
    //outColor = texture(texSampler, fragTexCoord); // pic only
    // outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, 1.0); // texture colors with vertex colors

    vec3 N = normalize(fragNormal);
    vec3 L = normalize(fragLightVec);
    vec3 V = normalize(fragViewVec);
    vec3 R = reflect(L, N);

    vec3 ambient = fragColor * 0.5;
    vec3 diffuse = max(0.0, dot(N, L)) * fragColor;
    vec3 specular = pow(max(0.0, dot(R,V)), 16.0) * vec3(1.35);

    outColor = vec4( ambient + diffuse + specular, 1.0f);
}
