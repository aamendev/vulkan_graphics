#version 450

layout (location = 0) in vec4 inPosition;
//layout (location = 1) in vec2 texs;
//layout (location = 2) in vec3 norm;
//layout (location = 1) in vec3 inColour;

//layout(location = 0) out vec3 fragColor;

//layout(location = 1) out vec2 fragTexCoord; 

layout(push_constant) uniform pushConstant
{
    mat4 transMat;
} ps;

void main() {
    gl_Position = ps.transMat * inPosition;
 //   fragTexCoord = texs;
 //   fragColor = inColour;
}
