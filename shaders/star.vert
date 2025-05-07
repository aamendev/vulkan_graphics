layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec2 idx;
layout (location = 1) out vec3 fragNorm; 
layout (location = 2) out vec3 fragPos; 


layout(push_constant) uniform pushConstant
{
    mat4 transMat;
    mat4 proj;
} ps;

void main() {
    vec4 val = ps.transMat * vec4(inPosition.xyz, 1);
    gl_Position = ps.proj * val;
    mat4 normalTrans = transpose(inverse(ps.transMat));
    fragNorm = normalize(normalTrans * vec4(inNormal.xyz, 1)).xyz;
    fragPos = val.xyz;
}

