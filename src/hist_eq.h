#ifndef HIST_EQ_H
#define HIST_EQ_H

#include <v3d_interface.h>

/// MIT license
/// by Zuohan Zhao


template <typename T>
void thr_hist_eq(T* buffer, V3DLONG size, V3DLONG n_bin, float thr)
{
    T max = buffer[0], min = buffer[0];
    for (V3DLONG i = 0; i < size; ++i)
    {
        max = std::max(buffer[i], max);
        min = std::min(buffer[i], min);
    }

    T bin_size = 1;
    if (n_bin == 0)
        n_bin = max - min + 1;
    else
        bin_size = (max - min) / n_bin;

    QList<V3DLONG> hist(n_bin);
    QList<float> cumu(n_bin);

    for (V3DLONG i = 0; i < size; ++i)
        ++hist[V3DLONG((buffer[i] - min) / bin_size)];

    auto&& thr_bin = V3DLONG((thr - min) / bin_size);
    cumu[thr_bin]=hist[thr_bin];
    for (V3DLONG i = thr_bin + 1; i < n_bin; ++i)
        cumu[i] = cumu[i-1] + hist[i];
    auto&& range = max - thr;
    for (V3DLONG i = thr_bin; i < n_bin; ++i)
        cumu[i] = cumu[i] * range / cumu.back() + thr;

    for (V3DLONG i = 0; i < size; ++i)
        if (buffer[i] >= thr)
            buffer[i] = cumu[V3DLONG((buffer[i] - min) / bin_size)];
}

#endif // HIST_EQ_H
