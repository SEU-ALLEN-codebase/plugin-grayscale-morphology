#ifndef ADA_THR_H
#define ADA_THR_H

#include <v3d_interface.h>

template <class T>
void ada_thr(T* apsInput, T* aspOutput, V3DLONG sz[4], V3DLONG h[3], V3DLONG d)
{
    V3DLONG&& mCount = sz[0] * sz[1];
    for (V3DLONG z = 0; z < sz[2]; ++z)
        for (V3DLONG y = 0; y < sz[1]; ++y)
            for (V3DLONG x = 0; x < sz[0]; ++x)
            {
                V3DLONG curpos = z * mCount + y*sz[0] + x;
                V3DLONG curpos1 = z* mCount + y*sz[0];
                V3DLONG curpos2 = y* sz[0] + x;
                float temp = 0;
                auto count = 0;
                for(V3DLONG n = 1; n <= d; ++n)
                {
                    if (x > h[0] * n)                  // x-
                    {
                        temp += apsInput[curpos1 + x - (h[0] * n)];
                        ++count;
                    }
                    if (x + (h[0] * n) < sz[0])        // x+
                    {
                        temp += apsInput[curpos1 + x + (h[0] * n)];
                        ++count;
                    }
                    if (y > h[1] * n)                  // y-
                    {
                        temp += apsInput[z * mCount + (y - (h[1] * n)) * sz[0] + x];
                        ++count;
                    }
                    if (y + (h[1] * n) < sz[1])        // y+
                    {
                        temp += apsInput[z * mCount + (y + (h[1] * n)) * sz[0] + x];
                        ++count;
                    }
                    if (z > (h[2] * n))                // z-
                    {
                        temp += apsInput[(z - (h[2] * n)) * mCount + curpos2];
                        ++count;
                    }
                    if (z + (h[2] * n) < sz[2])        // z+
                    {
                        temp += apsInput[(z + (h[2] * n)) * mCount + y * sz[0] + x];
                        ++count;
                    }

                    // flare suppress
                    if (x > h[0] * n && z + (h[2] * n) < sz[2])        // z+x-
                    {
                        temp += apsInput[(z + (h[2] * n)) * mCount + y * sz[0] + x - (h[0] * n)];
                        ++count;
                    }
                    if (x + (h[0] * n) < sz[0] && z + (h[2] * n) < sz[2])        // z+x+
                    {
                        temp += apsInput[(z + (h[2] * n)) * mCount + y * sz[0] + x + (h[0] * n)];
                        ++count;
                    }
                    if (y > h[1] * n && z + (h[2] * n) < sz[2])        // z+y-
                    {
                        temp += apsInput[(z + (h[2] * n)) * mCount + (y - (h[1] * n)) * sz[0] + x];
                        ++count;
                    }
                    if (y + (h[1] * n) < sz[1] && z + (h[2] * n) < sz[2])        // z+y+
                    {
                        temp += apsInput[(z + (h[2] * n)) * mCount + (y + (h[1] * n)) * sz[0] + x];
                        ++count;
                    }
                }
                auto&& t =  apsInput[curpos] - temp / count;
                aspOutput[curpos]= (t > 0)? t : 0;
            }
}


#endif // ADA_THR_H
