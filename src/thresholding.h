#ifndef THRESHOLDING_H
#define THRESHOLDING_H
#include <v3d_interface.h>

/// MIT license
/// by Zuohan Zhao


template <typename T>
float percentile_threshold(T* buffer, V3DLONG size, float percentile)
{
    if (percentile < 0 || percentile > 1)
        throw "Percentiel threshold: percentile invalid.";
    QSharedPointer<T> temp_buffer(new T[size], std::default_delete<T[]>());
    auto&& temp = temp_buffer.data();
    memcpy(temp, buffer, size * sizeof(T));
    std::sort(temp, temp + size, std::greater<T>());
    return temp[std::clamp(lround(percentile * size), 0l, size - 1)];
}


/// modified from Fiji plugin
template <typename T>
float triangle_threshold(T* buffer, V3DLONG size, V3DLONG n_bin, float start_thr)
{
    /// locate grayscale min/max value
    T max = buffer[0], min = buffer[0];
    for (V3DLONG i = 0; i < size; ++i)
    {
        min = std::min(min, buffer[i]);
        max = std::max(max, buffer[i]);
    }

    /// pass n_bin = 0 means no binning is done
    T bin_size = 1;
    if (n_bin == 0)
        n_bin = max - min + 1;
    else
        bin_size = (max - min) / n_bin;

    QList<V3DLONG> hist(n_bin);
    for (V3DLONG i = 0; i < size; ++i)
        ++hist[V3DLONG((buffer[i] - min) / bin_size)];

    /// locate max count grayscale
    V3DLONG peak = 0;
    for (V3DLONG i = 0; i < hist.size(); ++i)
        if (hist[i] >= hist[peak])
            peak = i;

    /// test distance
    /// nx * x + ny * y - d = 0
    float&& nx = hist.back() - hist[peak];
    float&& ny = peak - n_bin + 1;
    float&& d = std::sqrt(nx * nx + ny * ny);
    nx /= d;
    ny /= d;
    d = nx * (n_bin - 1) + ny * hist.back();
    float dist = 0;
    V3DLONG split;
    for (V3DLONG i = std::max(peak + 1, V3DLONG((start_thr - min) / bin_size)); i < hist.size(); ++i)
    {
        auto&& new_dist = nx * i + ny * hist[i] - d;
        if (new_dist > dist)
        {
            dist = new_dist;
            split = i;
        }
    }
    return split * bin_size;
}

#endif // THRESHOLDING_H
