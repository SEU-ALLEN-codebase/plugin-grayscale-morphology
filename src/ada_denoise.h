#ifndef ADA_DENOISE_H
#define ADA_DENOISE_H

#include <v3d_interface.h>

template <class T>
void ada_denoise(const T* const src, T* const dst, const V3DLONG sz[4],
                 const V3DLONG h[3], const V3DLONG d, const V3DLONG hh[3], const V3DLONG dd,
                 const float flare_weight, const float atten_depth, bool flare_x, bool flare_y)
{
    qDebug() << "Adaptive Threshold:";
    qDebug() << "Stride =" << h[0] << h[1] << h[2];
    qDebug() << "Sampling =" << d;
    qDebug() << "Flare Removal:";
    qDebug() << "Stride =" << hh[0] << hh[1] << hh[2];
    qDebug() << "Sampling =" << dd;
    qDebug() << "Weight =" << flare_weight;
    qDebug() << "Attenuation Depth =" << atten_depth;

    // attenuation weights
    QList<float> w(dd + 1);     // start from 1
    {
        float sum = 0;
        for (V3DLONG n = 1; n <= dd; ++n)
        {
            const auto& depth = n * hh[2] / atten_depth;
            w[n] = 1 / (depth*depth + 1);
            sum += w[n];
        }
        qDebug() << "Weights:";
        for (V3DLONG n = 1; n <= dd; ++n)
        {
            w[n] *= flare_weight / sum;
            printf("%f\t", w[n]);
            if (n % 5 == 0 && n < dd) printf("\n");
        }
        printf("\n");
    }

    const auto& mCount = sz[0] * sz[1];
    for (V3DLONG z = 0; z < sz[2]; ++z)
    {
        for (V3DLONG y = 0; y < sz[1]; ++y)
            for (V3DLONG x = 0; x < sz[0]; ++x)
            {
                const auto& curpos = z * mCount + y * sz[0] + x;
                const auto& curpos1 = z * mCount + y * sz[0];
                const auto& curpos2 = y * sz[0] + x;

                // adaptive threshold
                float ada_sum = 0;
                int ada_count = 0;
                for(V3DLONG n = 1; n <= d; ++n)
                {
                    if (x > h[0] * n)                  // x-
                    {
                        ada_sum += src[curpos1 + x - (h[0] * n)];
                        ++ada_count;
                    }
                    if (x + h[0] * n < sz[0])        // x+
                    {
                        ada_sum += src[curpos1 + x + (h[0] * n)];
                        ++ada_count;
                    }
                    if (y > h[1] * n)                  // y-
                    {
                        ada_sum += src[z * mCount + (y - (h[1] * n)) * sz[0] + x];
                        ++ada_count;
                    }
                    if (y + h[1] * n < sz[1])        // y+
                    {
                        ada_sum += src[z * mCount + (y + (h[1] * n)) * sz[0] + x];
                        ++ada_count;
                    }
                    if (z > h[2] * n)                // z-
                    {
                        ada_sum += src[(z - (h[2] * n)) * mCount + curpos2];
                        ++ada_count;
                    }
                    if (z + h[2] * n < sz[2])        // z+
                    {
                        ada_sum += src[(z + (h[2] * n)) * mCount + y * sz[0] + x];
                        ++ada_count;
                    }
                }

                // flare removal
                float fx = 0, fy = 0;
                for(V3DLONG n = 1; n <= dd; ++n)
                {
                    if (flare_x)
                    {
                        if (x > hh[0] * n && z + hh[2] * n < sz[2])                 // z+x-
                            fx += w[n] * src[(z + hh[2] * n) * mCount + y * sz[0] + x - hh[0] * n];
                        if (x + hh[0] * n < sz[0] && z + hh[2] * n < sz[2])         // z+x+
                            fx += w[n] * src[(z + hh[2] * n) * mCount + y * sz[0] + x + hh[0] * n];
                        if (z + hh[2] * n < sz[2])                                  // z+
                            fx += w[n] * src[(z + hh[2] * n) * mCount + y * sz[0] + x];
                    }
                    if (flare_y)
                    {
                        if (y > hh[1] * n && z + hh[2] * n < sz[2])                 // z+y-
                            fy += w[n] * src[(z + hh[2] * n) * mCount + (y - hh[1] * n) * sz[0] + x];
                        if (y + hh[1] * n < sz[1] && z + hh[2] * n < sz[2])         // z+y+
                            fy += w[n] * src[(z + hh[2] * n) * mCount + (y + hh[1] * n) * sz[0] + x];
                        if (z + hh[2] * n < sz[2])                                  // z+
                            fy += w[n] * src[(z + hh[2] * n) * mCount + y * sz[0] + x];
                    }
                }
                dst[curpos] = std::max(src[curpos] - ada_sum / ada_count - std::max(fx, fy), 0.f);
            }
        printf("\r%d %% completed..", ((z + 1)*100) / sz[2]); fflush(stdout);
    }
    printf("\n");
}

#endif // ADA_DENOISE_H
