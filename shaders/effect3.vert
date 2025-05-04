#version 450 

layout (location = 0) out vec4 oldPos;
layout (location = 0) in vec4 inPosition;

layout(push_constant) uniform pushConstant
{
    mat4 transMat;
} ps;

void main() 
{
    oldPos = inPosition;
    gl_Position = ps.transMat * vec4(inPosition.xyz, 1);
}
