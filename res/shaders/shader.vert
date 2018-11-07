#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 color;

layout (location = 0) out vec3 fragColor;

layout (binding = 0) uniform UBO
{
    mat4 MVP;
} ubo;

void main()
{
    gl_Position = ubo.MVP * vec4(pos, 0.0, 1.0);
    fragColor = color;
}