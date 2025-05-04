#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 oldPos;
void main()
{
    float alpha = oldPos.w;
    if (oldPos.x * oldPos.x + oldPos.z * oldPos.z < 0.12f)
    {
        alpha = 0;
    }
    outColor = vec4(oldPos.w, oldPos.w, oldPos.w, alpha);
}

