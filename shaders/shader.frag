#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(binding = 1) uniform sampler2D texSampler;

// which framebuffer to put the color
layout(location = 0) out vec4 outColor;

void main()
{
    // outColor = vec4(fragTexCoord, 0.0, 1.0); //RGBAlpha, colors only
    outColor = texture(texSampler, fragTexCoord); // pic only
    // outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, 1.0); // texture colors with vertex colors
}
