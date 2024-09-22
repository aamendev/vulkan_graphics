#version 450

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColour;
layout (location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0 ) uniform fullMat{
    mat4 transMat;
} full;

layout(push_constant) uniform pushConstant
{
    mat4 transMat;
} ps;

void main() {
    gl_Position = ps.transMat * inPosition;
    fragColor = inColour;
    fragTexCoord = inTexCoord;
}
