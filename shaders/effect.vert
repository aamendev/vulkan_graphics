#version 450 

layout (location = 0) out vec2 outUV;
layout (location = 0) in vec2 inPosition;

void main() 
{
    outUV = vec2(((gl_VertexIndex << 1) & 2) - 1.0f, (gl_VertexIndex & 2) - 1.0f);
    gl_Position = vec4(inPosition.x, inPosition.y, 0.0, 1); 
}


