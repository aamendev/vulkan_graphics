#version 450

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform fullColor{
    layout(offset = 64) vec4 col;
} fullColour;
void main() {
    outColor = fullColour.col ;
}
