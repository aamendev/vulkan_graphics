#version 450

layout(location = 0) out vec4 outColor;

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;

layout (push_constant) uniform raytracedata
{
    vec4 rayOrigin;
    vec4 rayDir;
    vec4 rayUp;
    vec4 sphereCenter;
    float rad ;
} tracedata;

vec3 getDir(vec2 p)
{
    float d = 1000.0f;
    vec3 raybase = tracedata.rayOrigin.xyz;
    vec3 rayDir = tracedata.rayDir.xyz;
    vec3 rayUp = tracedata.rayUp.xyz;

    vec3 w = raybase - rayDir;
    w = normalize(w);
    vec3 u = cross(rayUp, w);
    u = normalize(u);
    vec3 v = cross(w, u);
    
    vec3 ret = u * p.x + v * p.y + w * -d;
    return normalize(ret);
}

float sphere_intersects()
{
    vec3 center = tracedata.sphereCenter.xyz;
    vec3 raybase = tracedata.rayOrigin.xyz;
    vec3 rayDir = getDir(vec2(gl_FragCoord.x, gl_FragCoord.y));
    float rad = tracedata.rad; 
    float ret = 0;

    vec3 base_center = center - raybase;
    float tca = dot(base_center, rayDir);

    if (tca < 0 &&
            dot(base_center, base_center) > rad * rad)
        return -1.0f;
    vec3 rejected = base_center - rayDir * (dot(base_center, rayDir));

    float thc = sqrt(rad * rad - dot(rejected, rejected));
    if (dot(rejected, rejected) <= rad * rad)
    {
        ret = tca - thc;
    }
    else 
    {
        ret = -1;
    }
    return ret; 
}

void main() {
    vec3 oldCol;
    oldCol = subpassLoad(inputColor).rgb;    
    if (sphere_intersects() == -1.0f)
    {
        oldCol = vec3(0,0,0);
    }
    outColor = vec4(oldCol, 1.0f);
}
