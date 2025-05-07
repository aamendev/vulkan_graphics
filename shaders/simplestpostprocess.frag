#version 450

layout(location = 0) out vec4 outColor;

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;

void main() {
    vec3 oldCol;
    oldCol = subpassLoad(inputColor).rgb;    
    outColor = vec4(oldCol, 1.0f);
}
