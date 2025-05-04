#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform fullColor{
    vec4 col;
} fullColour;

void main() {
    outColor = fullColour.col;
}
