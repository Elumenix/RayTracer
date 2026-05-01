#include "Noise.h"
#include <random>

namespace
{
    int FastFloor(float f) { return (f >= 0 ? (int)f : (int)f - 1); }
    float Lerp(float a, float b, float t) { return a + t * (b - a); }
    float InterpHermiteFunc(float t) { return t * t * (3 - 2 * t); }
    float InterpQuinticFunc(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

    const float GRAD_X[] =
        {
            1, -1, 1, -1,
            1, -1, 1, -1,
            0, 0, 0, 0};
    const float GRAD_Y[] =
        {
            1, 1, -1, -1,
            0, 0, 0, 0,
            1, -1, 1, -1};
    const float GRAD_Z[] =
        {
            0, 0, 0, 0,
            1, 1, -1, -1,
            1, 1, -1, -1};
}

namespace Noise
{
    void Perlin::SetSeed(int seed)
    {
        m_seed = seed;

        std::mt19937_64 gen(seed);

        for (int i = 0; i < 256; i++)
            m_perm[i] = i;

        for (int j = 0; j < 256; j++)
        {
            int rng = (int)(gen() % (256 - j));
            int k = rng + j;
            int l = m_perm[j];
            m_perm[j] = m_perm[j + 256] = m_perm[k];
            m_perm[k] = l;
            m_perm12[j] = m_perm12[j + 256] = m_perm[j] % 12;
        }
    }

    float Perlin::Sample(unsigned char offset, float x, float y, float z) const
    {
        int x0 = FastFloor(x);
        int y0 = FastFloor(y);
        int z0 = FastFloor(z);
        int x1 = x0 + 1;
        int y1 = y0 + 1;
        int z1 = z0 + 1;

        float xs, ys, zs;
        /*switch (m_interp)
        {
        case Linear:*/
        xs = x - (float)x0;
        ys = y - (float)y0;
        zs = z - (float)z0;
        /*  break;
      case Hermite:
          xs = InterpHermiteFunc(x - (float)x0);
          ys = InterpHermiteFunc(y - (float)y0);
          zs = InterpHermiteFunc(z - (float)z0);
          break;
      case Quintic:
          xs = InterpQuinticFunc(x - (float)x0);
          ys = InterpQuinticFunc(y - (float)y0);
          zs = InterpQuinticFunc(z - (float)z0);
          break;
      }*/

        float xd0 = x - (float)x0;
        float yd0 = y - (float)y0;
        float zd0 = z - (float)z0;
        float xd1 = xd0 - 1;
        float yd1 = yd0 - 1;
        float zd1 = zd0 - 1;

        float xf00 = Lerp(GradCoord3D(offset, x0, y0, z0, xd0, yd0, zd0), GradCoord3D(offset, x1, y0, z0, xd1, yd0, zd0), xs);
        float xf10 = Lerp(GradCoord3D(offset, x0, y1, z0, xd0, yd1, zd0), GradCoord3D(offset, x1, y1, z0, xd1, yd1, zd0), xs);
        float xf01 = Lerp(GradCoord3D(offset, x0, y0, z1, xd0, yd0, zd1), GradCoord3D(offset, x1, y0, z1, xd1, yd0, zd1), xs);
        float xf11 = Lerp(GradCoord3D(offset, x0, y1, z1, xd0, yd1, zd1), GradCoord3D(offset, x1, y1, z1, xd1, yd1, zd1), xs);

        float yf0 = Lerp(xf00, xf10, ys);
        float yf1 = Lerp(xf01, xf11, ys);

        return Lerp(yf0, yf1, zs);
    }

    float Perlin::Sample(unsigned char offset, Math::Point p) const
    {
        return Sample(offset, p.x, p.y, p.z);
    }

    float Perlin::GradCoord3D(unsigned char offset, int x, int y, int z, float xd, float yd, float zd) const
    {
        unsigned char lutPos = Index3D_12(offset, x, y, z);

        return xd * GRAD_X[lutPos] + yd * GRAD_Y[lutPos] + zd * GRAD_Z[lutPos];
    }
}
