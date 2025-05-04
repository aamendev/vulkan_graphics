#include "modifier.h"
namespace Lina{ namespace Graphics{
    void Modifier::init()
    {
        mGradientTab.resize(TABSIZE * 3);
        initGradientTable();
    }
    void Modifier::initGradientTable()
    {
        float z, r, theta;
        int i;
        r = 1.0f;
        for (i = 0; i < TABSIZE; i++)
        {
            z = 1.0 - 2 * RANDNBR;
            r = sqrt(fabs(1 - z*z));
            theta = 2 * M_PI * RANDNBR;
            mGradientTab[(i * 3)] = r * cos(theta);
            mGradientTab[(i * 3) + 1] = r * sin(theta);
            mGradientTab[(i * 3) + 2] = z;
        }
    }
    f32 Modifier::glattice(int ix, int iy, int iz, f32 fx, f32 fy, f32 fz)
    {
        int pos = (INDEX(ix, iy, iz)) * 3;
        float temp = mGradientTab[pos] * fx + mGradientTab[pos + 1] * fy + mGradientTab[pos + 2] * fz;
        return temp;
    }
    f32 Modifier::gnoise(f32 x, f32 y, f32 z)
    {
        int ix, iy, iz;
        float fx, fy, fz;
        float wx, wy, wz;
        float vx0, vx1, vy0, vy1, vz0, vz1;

        ix = Core::Utils::floor(x);
        fx = x - ix;
        wx = SMOOTH(fx);

        iy = Core::Utils::floor(y);
        fy = y - iy;
        wy = SMOOTH(fy);

        iz = Core::Utils::floor(z);
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

    void Modifier::init_fbm(float h, float lac)
    {
        float frequency;
        int i;
        frequency = 1.0;
        for (i=0; i<MAX_OCTAVES; i++) 
        {
            mExponentTab[i] = pow(frequency, -h);
            frequency *= lac;
        }
        mInit = true;
    }

    f32 Modifier::perPointFbm(Math::Point3D point, f32 h, f32 lac, f32 octaves)
    {
        float value, remainder;
        int i;
        value = 0.0;
        for (i=0; i< octaves; i++) {
            value += gnoise(point.x, point.y, point.z) * mExponentTab[i];
            point.x *= lac;
            point.y *= lac;
            point.z *= lac;
        }
        remainder = octaves - Core::Utils::floor(octaves);
        value += (remainder > 0) * remainder * gnoise(point.x, point.y, point.z) * mExponentTab[i];
        return(value);
    }

    f32 Modifier::perPointWarp(Math::Point3D p, f32 h, f32 lac, f32 octaves, f32 dist)
    {
        auto temp = p;
        Math::Point3D distortedPoint;
        distortedPoint.x = perPointFbm(temp, h, lac, octaves);
        //    temp.x += 10.5f;
        temp.x += 0.0f;
        distortedPoint.y = perPointFbm(temp, h, lac, octaves);
        //temp.y += 10.5f;
        temp.y += 0.0f;
        distortedPoint.z = perPointFbm(temp, h, lac, octaves);
        p.x += dist * distortedPoint.x;
        p.y += dist * distortedPoint.y;
        p.z += dist * distortedPoint.z;

        return perPointFbm(p, h, lac, octaves);
    }

    f32 Modifier::perPointHybridMultiFractal(Math::Point3D p, 
            f32 h, f32 lac, f32 octaves, f32 offset)
    {
        f32 value, remainder, signal, weight;
        value = (gnoise(p.x, p.y, p.z) + offset) * mExponentTab[0];
        weight = value;
        p.x *= lac;
        p.y *= lac;
        p.z *= lac;

        for (int i = 1; i < octaves; i++)
        {
            weight = weight * !(weight > 1.0) + 1.0 * (weight > 1.0);
            signal = (gnoise(p.x, p.y, p.z) + offset) * mExponentTab[i];
            value += weight * signal;
            weight *= signal;
            p.x *= lac;
            p.y *= lac;
            p.z *= lac;
        }
        remainder = octaves - Core::Utils::floor(octaves);
        value += (remainder > 0) * remainder * gnoise(p.x, p.y, p.z) * mExponentTab[octaves];
        return value;
    }
}}
