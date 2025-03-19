#version 450

layout(location = 0) out vec4 outColor;

layout(location = 3) in vec3 fragColor;

layout(binding = 0 ) uniform fullColor{
    vec4 col;
} fullColour;

void main() {
    outColor = vec4(fragColor, 1.0f) * fullColour.col; 
}
