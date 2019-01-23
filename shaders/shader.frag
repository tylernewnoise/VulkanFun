#version 450
#extension GL_ARB_separate_shader_objects : enable

// which framebuffer to put the color
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(0.0, 1.0, 0.0, 1.0); //RGBAlpha
}
