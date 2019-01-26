#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
// which framebuffer to put the color
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fragColor, 1.0); //RGBAlpha
}
