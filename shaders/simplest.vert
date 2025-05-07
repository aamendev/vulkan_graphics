#version 450

layout (location = 0) in vec4 inPosition;

layout(push_constant) uniform pushConstant
{
    mat4 transMat;
} ps;

void main() {
    gl_Position = ps.transMat * inPosition;
}
