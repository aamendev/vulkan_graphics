#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 idx;
layout (location = 1) in vec3 fragNorm; 
layout (location = 2) in vec3 fragPos; 


layout(binding = 0) uniform fullColor{
    vec4[3] col;
} fullColour;

layout (binding = 1) uniform lights
{
    vec4 ambLight;
    vec4 lightPos;
    vec4 lightCol;
} light;

void main() {
    int val = int(idx.x);
    vec3 dirToLight = light.lightPos.xyz - fragPos;
    float atten = 1.0f / dot(dirToLight, dirToLight);
    //atten = 0.5f;
    vec3 lightColour = light.lightCol.xyz * light.lightCol.w * atten;
    vec3 ambient = light.ambLight.xyz * light.ambLight.w;
    vec3 diffuse = lightColour * max(dot(normalize(-fragNorm), normalize(dirToLight)), 0);
    outColor = vec4((diffuse+ambient) * fullColour.col[val].xyz, fullColour.col[val].w);
}
