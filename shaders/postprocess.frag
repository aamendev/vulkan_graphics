#version 450

layout(location = 0) out vec4 outColor;

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;

layout (push_constant) uniform raytracedata
{
    vec4 rayOrigin;
    vec4 rayLookAt;
    vec4 rayUp;
    vec4 sphere;
    vec2 dim;
} tracedata;

vec3 getDir(vec2 p)
{
    float d = 1.0f;
    vec3 raybase = tracedata.rayOrigin.xyz;
    vec3 rayLookAt = tracedata.rayLookAt.xyz;
    vec3 rayUp = tracedata.rayUp.xyz;

    vec3 w = raybase - rayLookAt;
    w = normalize(w);
    vec3 u = cross(rayUp, w);
    u = normalize(u);
    vec3 v = cross(w, u);
    
    vec3 ret = u * p.x + v * p.y + w * -d;
    return normalize(ret);
}

vec2 sphere_intersects()
{
    vec3 center = tracedata.sphere.xyz;
    vec3 raybase = tracedata.rayOrigin.xyz;
    vec3 rayDir = getDir(vec2(-(gl_FragCoord.x - tracedata.dim.x/2) / tracedata.dim.x, 
    (gl_FragCoord.y - tracedata.dim.y/2) / tracedata.dim.y));
    float rad = tracedata.sphere.w; 
    vec2 ret;

    vec3 base_center = center - raybase;
    float tca = dot(base_center, rayDir);

    if (tca < 0 &&
            dot(base_center, base_center) > rad * rad)
        return vec2(-1./0, -1./0);
    vec3 rejected = base_center - rayDir * (dot(base_center, rayDir));

    float thc = sqrt(rad * rad - dot(rejected, rejected));
    if (dot(rejected, rejected) <= rad * rad)
    {
        ret.x = tca - thc;
        ret.y = tca + thc;
    }
    else 
    {
        ret = vec2(-1./0, -1./0);
    }
    return ret; 
}

void main() {
    vec3 oldCol;
    oldCol = subpassLoad(inputColor).rgb;    
    vec2 ret = sphere_intersects();
    if (isinf(ret.x))
    {
        oldCol = vec3(0,0,0);
        outColor = vec4(oldCol, 1.0f);
    }
    else 
    {
        outColor = vec4(oldCol, 1.0f);
        //outColor = vec4(oldCol, abs(ret.y - ret.x) / (2 * tracedata.sphere.w));
    }
}
