#version 450

#define LERP(t, x0, x1) ((x0) + (t) * ((x1) - (x0)))
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler texSampler;
layout(set=0, binding = 1) uniform texture3D textures;

layout (input_attachment_index = 0, set = 0, binding = 2) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 0, binding = 3) uniform subpassInput inputDepth;

float linearize_depth(float d, float zNear,float zFar)
{
    return zNear * zFar / (zFar + d * (zNear - zFar));
}

float get_depth(float d, float zNear, float zFar)
{
    return zNear + (zFar - zNear) * d;
}

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
    float d = 0.21f;
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
    float oldDepth = get_depth(subpassLoad(inputDepth).r, 0.0f, 100.0f);
    vec3 center = tracedata.sphere.xyz;
    vec3 raybase = tracedata.rayOrigin.xyz;
    vec3 rayDir = getDir(vec2(-(gl_FragCoord.x + 0.5f - tracedata.dim.x/2) / tracedata.dim.x, 
    (gl_FragCoord.y + 0.5f - tracedata.dim.y/2) / tracedata.dim.y));
    float rad = tracedata.sphere.w; 
    vec2 ret;

    vec3 base_center = center - raybase;
    float tca = dot(base_center, rayDir);

    if ((tca < 0 &&
            dot(base_center, base_center) > rad * rad))
    {
        return vec2(-1./0, -1./0);
    }
    vec3 rejected = base_center - rayDir * (dot(base_center, rayDir));

    float thc = sqrt(rad * rad - dot(rejected, rejected));
    if (dot(rejected, rejected) <= rad * rad)
        //&& dot(rejected, rejected) < oldDepth)
    {
        ret.x = tca - thc;
        ret.y = tca + thc;
        if (ret.x > oldDepth)
        {
            ret = vec2(-1./0, -1./0);
        }
    }
    else 
    {
        ret = vec2(-1./0, -1./0);
    }
    return ret; 
}

float getDensity(vec2 intDistance, int numSamples)
{
    float range = intDistance.y - intDistance.x;
    float d = tracedata.sphere.w * 2;
    float curr = 0;
    vec3 raybase = tracedata.rayOrigin.xyz;
    vec3 rayDir = normalize(tracedata.rayLookAt.xyz - tracedata.rayOrigin.xyz);
    for (int i = 0; i < numSamples; i++)
    {
        float current = intDistance.x + i * (range / float(numSamples - 1));
        vec3 tp1 = tracedata.rayOrigin.xyz + rayDir * current;
        vec3 fromOrg = tp1 - tracedata.sphere.xyz;
        float zCoord =  i * (range / float(numSamples - 1)) / d;
        vec3 texCoords = vec3((fromOrg.x + d/2) / d, (fromOrg.y + d/2)/d, zCoord);
        float texval = texture(sampler3D(textures, texSampler), texCoords).r; 
        if (texval == 0)
        {
            break;
        }
        curr = texval + (1-texval) * curr;
    }
    if (curr == 0)
    {
        curr = -1/0;
    }
    return 1 - curr;
}
void main() {
    vec3 oldCol;
    oldCol = subpassLoad(inputColor).rgb;    
    vec2 ret = sphere_intersects();
    
    float alpha = 1.0f;
    if (!isinf(ret.x))
    {
        float dens = getDensity(ret, 8);
        if (!isinf(dens))
        {
            alpha = exp(-dens);
            oldCol.x = LERP(alpha, oldCol.x, dens);
            oldCol.y = LERP(alpha, oldCol.y, dens);
            oldCol.z = LERP(alpha, oldCol.z, dens);
        }
        else 
        {
            oldCol.x = 1.0f;
            oldCol.y = 0.0f;
            oldCol.z = 0.0f;
        }
    }
    outColor = vec4(oldCol, alpha);
}
