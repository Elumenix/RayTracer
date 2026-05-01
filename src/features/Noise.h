// A lot of the implementation for this class came from Here:
// https://github.dev/charlesangus/FastNoise/
#pragma once
#include "Tuple.h"

namespace Noise
{
    class Perlin
    {
    public:
        Perlin(int seed = 1337)
        {
            SetSeed(seed);
        }

        void SetSeed(int seed);
        float Sample(unsigned char offset, float x, float y, float z) const;
        float Sample(unsigned char offset, Point p) const;

    private:
        int m_seed = 1337;
        unsigned char m_perm[512];
        unsigned char m_perm12[512];

        float GradCoord3D(unsigned char offset, int x, int y, int z, float xd, float yd, float zd) const;
        inline unsigned char Index3D_12(unsigned char offset, int x, int y, int z) const
        {
            return m_perm12[(x & 0xff) + m_perm[(y & 0xff) + m_perm[(z & 0xff) + offset]]];
        }
    };
}
