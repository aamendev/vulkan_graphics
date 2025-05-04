#version 460
#define PI (3.14159265359)
#define TWO_PI (PI*2)
#define M_PI (3.1415926535897932384626433832795)
#define Nnoise 4
#define LERP(t, x0, x1) ((x0) + (t) * ((x1) - (x0)))
#define SMOOTH(x) ((x) * (x) * (x) * ((x) * ((x) * 6 - 15) + 10));

layout (location = 0) in vec4 inPosition;

layout(push_constant) uniform pushConstant
{
    mat4 transMat;
} ps;


int FLOOR(float x)
{
    int correction;
    if (x < 0 && x != int(x))
    {
        correction = 1;
    }
    else
    {
        correction = 0;
    }
    return int(x) - correction; 
}

#define MAX_OCTAVES (10)
float exponent_array[MAX_OCTAVES];

#define TABSIZE (256)
const int  perm[256] =  {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7,
    225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134,
    139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
    105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80,
    73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
    147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
    28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181,
    199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236,
    205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

int next = 1;
int rand(void) 
{
    next = next * 1103515245 + 12345;
    return  int(uint(next/65536) % 32768);
}

#define TABMASK (TABSIZE - 1)
#define PERM(x) perm[(x) & TABMASK]
#define INDEX(ix, iy, iz) PERM(ix + PERM(iy + PERM(iz)))
#define RANDMASK 0xffffffff
#define RANDNBR (float((rand() & RANDMASK)) /  float(RANDMASK))
float gradientTab[TABSIZE * 3];

void gradTableInit(int seed)
{
    float z, r, theta;
    int i;
    r = 1.0f;
    next = seed;
    for (i = 0; i < TABSIZE; i++)
    {
        z = 1.0 - 2.0 * RANDNBR;
        r = sqrt(abs(1 - z*z));
        theta = 2 * M_PI * RANDNBR;
        gradientTab[(i * 3)] = r * cos(theta);
        gradientTab[(i * 3) + 1] = r * sin(theta);
        gradientTab[(i * 3) + 2] = z;
    }
}

float glattice(int ix, int iy, int iz,
        float fx, float fy, float fz)
{
    int pos = (INDEX(ix, iy, iz)) * 3;
    float temp = gradientTab[pos] * fx + gradientTab[pos + 1] * fy + gradientTab[pos + 2] * fz;
    if (isnan(temp))
    {
        return gradientTab[1];
    }
    return temp;
}
float gnoise(float x, float y, float z)
{
    int ix, iy, iz;
    float fx, fy, fz;
    float wx, wy, wz;
    float vx0, vx1, vy0, vy1, vz0, vz1;

    ix = FLOOR(x);
    fx = x - ix;
    wx = SMOOTH(fx);

    iy = FLOOR(y);
    fy = y - iy;
    wy = SMOOTH(fy);

    iz = FLOOR(z);
    fz = z - iz;
    wz = SMOOTH(fz);

    vx0 = glattice(ix, iy, iz, fx, fy, fz);
    if (isnan(vx0))
    {
        return (0.5f);
    }

    vx1 = glattice(ix + 1, iy, iz, fx - 1, fy, fz);
    vy0 = LERP(wx, vx0, vx1);

    vx0 = glattice(ix, iy + 1, iz, fx, fy - 1, fz);
    vx1 = glattice(ix + 1, iy + 1, iz, fx - 1, fy - 1, fz);
    vy1 = LERP(wx, vx0, vx1);
    vz0 = LERP(wy, vy0, vy1);

    vx0 = glattice(ix, iy, iz + 1, fx, fy, fz-1);
    vx1 = glattice(ix + 1, iy, iz + 1, fx-1, fy, fz-1);
    vy0 = LERP(wx, vx0, vx1);

    vx0 = glattice(ix, iy + 1, iz + 1, fx, fy-1, fz-1);
    vx1 = glattice(ix + 1, iy + 1, iz + 1, fx-1, fy-1, fz-1);
    vy1 = LERP(wx, vx0, vx1);
    vz1 = LERP(wy, vy0, vy1);

    float ret = LERP(wz, vz0, vz1);
    if (!isnan(ret))
    {
        return ret;
    }
    else 
    {
        return 22.9f;
    }
}

void init_fbm(float H, float lac)
{
    float frequency;
    int i;
    frequency = 1.0;
    for (i=0; i<MAX_OCTAVES; i++) 
    {
        exponent_array[i] = pow(frequency, -H);
        frequency *= lac;
    }
}

float
fbm(vec3 point, float H, float lacunarity, float octaves)
{
    float value, remainder;
    int i;
    value = 0.0;
    for (i=0; i<octaves; i++) {
        value += gnoise(point.x, point.y, point.z) * exponent_array[i];
        point.x *= lacunarity;
        point.y *= lacunarity;
        point.z *= lacunarity;
    }
    remainder = octaves - FLOOR(octaves);
    if (remainder > 0)
        value += remainder * gnoise(point.x, point.y, point.z) * exponent_array[i];
    return(value);
} 

void main()
{
    vec4 temp_pos = ps.transMat * inPosition;
    gradTableInit(656);
    float fac = 0.000001;
    init_fbm(0.25, 0.1);
    float new_y = fbm(temp_pos.xyz, 0.25, 1.2, 8.0);
    temp_pos.y =  new_y * fac;
    gl_Position = temp_pos;
}
