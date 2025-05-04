#version 460
#define PI 3.14159265359
#define TWO_PI (PI*2)
#define M_PI (3.1415926535897932384626433832795)
#define WATER (vec3(100. / 255, 216. / 255, 245. / 255))
#define PISTAGE (vec3(100. / 255, 245. / 255, 177. / 255))
#define BLUISH_GREEN (vec3(100. / 255, 245. / 255, 226. / 255))
#define BURNING_GREEN (vec3(100. / 255, 245. / 255, 128. / 255))
#define DARK_GREEN (vec3(72./255, 98./255, 86. / 255))
#define PALE_BLUE vec3(0.25, 0.25, 0.35)
#define MEDIUM_BLUE vec3(0.10, 0.10, 0.30)
#define DARK_BLUE vec3(0.05, 0.05, 0.26)
#define DARKER_BLUE vec3(0.03, 0.03, 0.20)
#define Nnoise 4

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

layout(binding = 0 ) uniform TIME
{
    vec2 dim;
    float time;
    float rand;
} stuff;

layout(push_constant) uniform pushConstant
{
  //layout(offset = 64)  vec2 dim;
    vec2 dim;
    float time;
} uniforms;

layout(location=0) out vec4 FragColor;


vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d) {
    return a + b*cos( 6.28318*(c*t+d) );
}
vec3 palette(float t, vec3 a, vec3 b)
{
    if (t > 0.5)
    {
        return a;
    }
    else
    {
        return b;
    }
}
vec3 palette(float t, int nums, vec3[4] cols)
{
    return cols[FLOOR(t * (float(nums) - 0.01))];
}

vec4 mandelbrot()
{
    float iterations = 1000;
    float fac = 10;
    float zoom = 1 / (exp(uniforms.time) * fac);
    float shift = -0.01;
    float x0 = 2.47 * (zoom * gl_FragCoord.x / uniforms.dim.x) - 0.05 - shift*1.3;
    float y0 = 2.24 * (zoom * gl_FragCoord.y / uniforms.dim.y) - 0.8 - shift*2;
    float currentX = 0;
    float currentY = 0;
    float bound = 4;
    float iter = 0;
    while (iter < iterations && currentX * currentX + currentY * currentY < bound)
    {
        float tempx = currentX * currentX - currentY * currentY;
        currentY = 2 * currentX * currentY + y0;
        currentX = tempx + x0;
        iter++;
    }
    return vec4
        (palette
         (iter/iterations, 
          vec3(0.02, 0.02, 0.03), 
          vec3(0.1, 0.2, 0.3), 
          vec3(0.0, 0.3, 0.2),
          vec3(0.0, 0.5, 0.8)), 1.0);
}
vec4 brick()
{
    float brickWidth = 0.25;
    float brickHeight = 0.08;
    float mortarThickness = 0.01;
    float bmWidth = brickWidth + mortarThickness;
    float bmHeight = brickHeight + mortarThickness;
    float mwf = mortarThickness*0.5/bmWidth;
    float mhf = mortarThickness*0.5 / bmHeight;

    vec4 brickColour = vec4(0.5, 0.15, 0.14, 1);
    vec4 mortarColour = vec4(0.5, 0.5, 0.5, 1);

    float ss = (gl_FragCoord.x / uniforms.dim.x) / bmWidth;
    float tt = (gl_FragCoord.y / uniforms.dim.y) / bmHeight;
    if (mod(tt*0.5, 1) > 0.5)
    {
        ss += 0.5;
    }
    ss -= floor(ss);
    tt -= floor(tt);
    float w = step(mwf, ss) - step(1 - mwf, ss);
    float h = step(mhf, tt) - step(1 - mhf, tt);
    return mix(mortarColour, brickColour, w*h);
}

vec4 julia()
{
    float r = 2;
    float zoom = 1 / sqrt(sqrt(uniforms.time));
    vec2 uv0 = 2 * r * zoom * gl_FragCoord.xy / uniforms.dim - r + 1.0f;
    float iter = 0;
    float iterations = 100;
    float speriod = 1;
    float cperiod = 1;
    float zx = cos(uniforms.time / speriod) * 0.7885;
    float zy = sin(uniforms.time / cperiod) * 0.7885;
    vec2 z = vec2(zx, zy);
    while (iter < iterations && uv0.x * uv0.x + uv0.y * uv0.y < r * r)
    {
        uv0 = vec2(uv0.x * uv0.x - uv0.y * uv0.y + z.x, 2 * uv0.x * uv0.y + z.y);
        iter++;
    }
    return vec4
        (palette
         (iter/iterations, 
          vec3(0.02, 0.02, 0.03), 
          vec3(0.1, 0.2, 0.3), 
          vec3(0.0, 0.3, 0.2),
          vec3(0.0, 0.5, 0.8)), 1.0);
}

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
    return  (next/65536) % 32768;
}
#define TABMASK (TABSIZE - 1)
#define PERM(x) perm[(x) & TABMASK]
#define INDEX(ix, iy, iz) PERM(ix + PERM(iy + PERM(iz)))
#define INDEX2(ix, iy) PERM(ix + PERM(iy))
#define RANDMASK 0xffffffff
#define RANDNBR (float((rand() & RANDMASK)) /  float(RANDMASK))

#define cr00 -0.5
#define cr01 1.5
#define cr02 -1.5
#define cr03 0.5
#define cr10 1.0
#define cr11 -2.5
#define cr12 2.0
#define cr13 -0.5
#define cr20 -0.50
#define cr21 0.0
#define cr22 0.5
#define cr23 0.0
#define cr30 0.0
#define cr31 1.0
#define cr32 0.0
#define cr33 0.0
float valueTab[TABSIZE];
float gradientTab[TABSIZE * 3];
float gradientTab2[TABSIZE * 2];
float rand(float seed)
{
    return fract(sin(seed) * 43758.5453);
}
void gradTableInit2(int seed, float r)
{
    float theta;
    int i;
    next = seed;
    for (i = 0; i < TABSIZE; i++)
    {
        theta = 2 * M_PI * RANDNBR;
        gradientTab2[(i * 2)] = r * cos(theta);
        gradientTab2[(i * 2) + 1] = r *  sin(theta);
    }
}
void gradTableInit(int seed)
{
    float z, r, theta;
    int i;
    next = seed;
    for (i = 0; i < TABSIZE; i++)
    {
        z = 1 - 2 * RANDNBR;
        r = sqrt(1 - z*z);
        theta = 2 * M_PI * RANDNBR;
        gradientTab[(i * 3)] = r * cos(theta);
        gradientTab[(i * 3) + 1] = r * sin(theta);
        gradientTab[(i * 3) + 2] = z;
    }
}
void valTableInit(int seed)
{
    next = seed;
    int i;
    for (i = 0; i < TABSIZE; i++)
    {
        valueTab[i] = 1 - 2 * RANDNBR;
    }
}

float glattice(int ix, int iy, int iz,
        float fx, float fy, float fz)
{
    int pos = (INDEX(ix, iy, iz)) * 3;
    return gradientTab[pos] * fx + gradientTab[pos + 1] * fy + gradientTab[pos + 2] * fz;
}
float glattice(int ix, int iy, float fx, float fy)
{
    int pos = (INDEX2(ix, iy)) * 2;
    return gradientTab2[pos] * fx + gradientTab2[pos + 1] * fy;
}

float vlattice(int ix, int iy, int iz)
{
    return valueTab[INDEX(ix, iy, iz)];
}
float vlattice(int ix, int iy)
{
    return valueTab[INDEX2(ix, iy)];
}
float spline(float x, int nknots, float[4] knots)
{
    int span;
    int nspans = nknots-3;
    if (nspans < 1)
    {
        return 0;
    }
    float c0, c1, c2, c3;
    x = clamp(x, 0, 1) * nspans;
    span = int(x);
    if (span >= nknots - 3)
    {
        span = nknots - 3;
    }
    x -= span;
    c3 = cr00*knots[span + 0] + cr01 * knots[span + 1] + cr02*knots[span + 2] + cr03*knots[span + 3];
    c2 = cr10*knots[span + 0] + cr11 * knots[span + 1] + cr12*knots[span + 2] + cr13*knots[span + 3];
    c1 = cr20*knots[span + 0] + cr21 * knots[span + 1] + cr22*knots[span + 2] + cr23*knots[span + 3];
    c0 = cr30*knots[span + 0] + cr31 * knots[span + 1] + cr32*knots[span + 2] + cr33*knots[span + 3];
    return ((c3 * x + c2) * x + c1) * x + c0;
}


float vnoise(float x, float y, float z)
{
    int ix, iy, iz;
    int i, j, k;
    float fx, fy, fz;
    float xknots[4], yknots[4], zknots[4];

    ix = FLOOR(x);
    fx = x - ix;
    iy = FLOOR(y);
    fy = y - iy;
    iz = FLOOR(z);
    fz = z - iz;
    for (k = -1; k < 3; k++)
    {
        for (j = -1; j < 3; j++)
        {
            for (i = -1; i < 3; i++)
            {
                xknots[i+1] = vlattice(ix + i, iy + j, iz + k);
            }

            yknots[j+1] = spline(fx, 4, xknots); 
        }
        zknots[k+1] = spline(fy, 4, yknots); 
    }
    return spline(fz, 4, zknots);
}
float vnoise(float x, float y)
{
    int ix, iy;
    int i, j;
    float fx, fy;
    float xknots[4], yknots[4];

    ix = FLOOR(x);
    fx = x - ix;
    iy = FLOOR(y);
    fy = y - iy;
    for (j = -1; j < 3; j++)
    {
        for (i = -1; i < 3; i++)
        {
            xknots[i+1] = vlattice(ix + i, iy + j);
        }

        yknots[j+1] = spline(fx, 4, xknots); 
    }
    return spline(fy, 4, yknots);
}
#define LERP(t, x0, x1) ((x0) + (t) * ((x1) - (x0)))
#define SMOOTH(x) ((x) * (x) * (x) * ((x) * ((x) * 6 - 15) + 10));

float gnoise(float x, float y)
{
    int ix, iy;
    float fx, fy;
    float wx, wy;
    float vx0, vx1, vy0, vy1;

    ix = FLOOR(x);
    fx = x - ix;
    wx = SMOOTH(fx);

    iy = FLOOR(y);
    fy = y - iy;
    wy = SMOOTH(fy);

    vx0 = glattice(ix, iy, fx, fy);
    vx1 = glattice(ix + 1, iy, fx - 1, fy);
    vy0 = LERP(wx, vx0, vx1);

    vx0 = glattice(ix, iy + 1, fx, fy - 1);
    vx1 = glattice(ix + 1, iy + 1, fx - 1, fy - 1);
    vy1 = LERP(wx, vx0, vx1);

    float ret = LERP(wy, vy0, vy1);
    return ret;
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
    return ret;
}
#define snoise(x, y) (2 * gnoise(x, y) - 1)

float fractalSum(float x, float y)
{
    int maxFreq = 16;
    int minFreq = 8;
    float value = 0;
    int f;
    for (f = minFreq; f < maxFreq; f *= 2)
    {
        value += abs(snoise(x * f, y * f)) / f;
    }
    return value;
}
void main()
{
   // vec3 cols[5] = {WATER, PISTAGE , BLUISH_GREEN, DARK_GREEN, BURNING_GREEN};

    gradTableInit2(656, 1);
    vec2 uv = gl_FragCoord.xy / uniforms.dim;
    float fac = 1;
    uv.x *= exp(-uniforms.time * 5);
    float c = fractalSum(uv.x * fac, uv.y * fac);
    vec3[4] cols = {PALE_BLUE, MEDIUM_BLUE, DARK_BLUE, DARKER_BLUE};
    //vec3 col2 = palette(c * 2, 4, cols);
    //vec3 col = vec3(c , c , c );
    vec3 col2 = vec3(c / 2, c / 2, c / 2);
    
    //vec4 col2 = julia();
    //col2.a = 0.75;
   FragColor = vec4(col2, 0.5);
   //FragColor = col2;
}

