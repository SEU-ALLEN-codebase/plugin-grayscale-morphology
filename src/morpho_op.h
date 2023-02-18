#ifndef MORPHO_OP_H
#define MORPHO_OP_H

#include <v3d_interface.h>
#include <QVector3D>

template <class T> void morpho_open(T* data1d,
                                      V3DLONG in_sz[4],
                                      QVector3D win,
                                      float thr)
{

    auto N = in_sz[0], M = in_sz[1], P = in_sz[2];
    auto&& pagesz = N*M*P;

    V3DLONG Wx = win[0], Wy = win[1], Wz = win[2];
    auto&& max_count = Wx*Wy*Wz;

    QSharedPointer<bool> bin_buffer(new bool[pagesz], std::default_delete<bool[]>());
    if (bin_buffer.isNull())
        throw "Morphological open: Memory allocation failed.";
    auto&& bin = bin_buffer.data();
    memset(bin, 0, sizeof(bool) * pagesz);

    for (V3DLONG iz = 0; iz < P; iz++)
    {
        V3DLONG offsetk = iz*M*N;
        for (V3DLONG iy = 0; iy < M; iy++)
        {
            V3DLONG offsetj = iy*N;
            for (V3DLONG ix = 0; ix < N; ix++)
            {

                auto xb = ix, yb = iy, zb = iz;
                auto xe = std::min(N, ix+Wx);
                auto ye = std::min(M, iy+Wy);
                auto ze = std::min(P, iz+Wz);

                if ((xe-xb)*(ye-yb)*(ze-zb) < max_count)      // at border, no copy
                    continue;
                V3DLONG count = 0;

                for(V3DLONG k=zb; k < ze; k++)
                {
                    V3DLONG offsetkl = k*M*N;
                    for(V3DLONG j=yb; j < ye; j++)
                    {
                        V3DLONG offsetjl = j*N;
                        for(V3DLONG i=xb; i < xe; i++)
                        {
                            T dataval = data1d[offsetkl + offsetjl + i];
                            if (dataval > thr)
                                ++count;
                        }
                    }
                }

                if (count == max_count)         // all foreground in the window
                    for(V3DLONG k=zb; k < ze; k++)
                    {
                        V3DLONG offsetkl = k*M*N;
                        for(V3DLONG j=yb; j < ye; j++)
                        {
                            V3DLONG offsetjl = j*N;
                            for(V3DLONG i=xb; i < xe; i++)
                                bin[offsetkl + offsetjl + i] = true;
                        }
                    }
            }
        }
    }
    for (V3DLONG iz = 0; iz < P; iz++)
    {
        V3DLONG offsetk = iz*M*N;
        for (V3DLONG iy = 0; iy < M; iy++)
        {
            V3DLONG offsetj = iy*N;
            for (V3DLONG ix = 0; ix < N; ix++)
                if (!bin[offsetk + offsetj + ix])
                    data1d[offsetk + offsetj + ix] = 0;
        }
    }
}


#endif // MORPHO_OP_H
