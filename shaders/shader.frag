#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec2 fragTexCoord;
layout(location = 0) in vec3 fragColor;

layout(binding = 1) uniform sampler texSampler;
layout(set=0, binding = 2) uniform texture2D textures[3];

layout(push_constant) uniform PER_OBJECT
{
   layout(offset = 64) int imgIdx;
} pc;

void main() {
    outColor = vec4(fragColor * texture(sampler2D(textures[pc.imgIdx], texSampler), fragTexCoord).rgb, 1.0);
}
