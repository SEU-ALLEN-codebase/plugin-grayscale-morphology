#ifndef EXPORT_H
#define EXPORT_H

#include <QVector3D>
#include "imPreProcess/fun_fft.h"
#include "imPreProcess/bilateral_filter.h"

/// Modified from v3d plugin imPreProcess by Shuxia Guo
/// All pointers are turned shared pointer
/// export everything in one template function
/// remove the rescaling, because it can cause trouble for triangle thershold


template <typename T>
void enhancement(T* buffer_in, T* buffer_out, const V3DLONG sz[4], int datatype, float cutoff, float gain, float color_sigma,
                    const QVector3D& sigma, bool bilateral, bool fft)
{
    sigma_correction(buffer_in,
                     sz,
                     cutoff,
                     gain,
                     buffer_out,
                     datatype);

    swap(buffer_in, buffer_out);
    subtract_min(buffer_in, sz, buffer_out);

    if (bilateral)
    {
        swap(buffer_in, buffer_out);
        auto&& radius = sigma * 3;
        int kernel_size[3] = {lround(radius[0])*2+1,
                              lround(radius[1])*2+1,
                              lround(radius[2])*2+1};
        bilateralfilter(buffer_in, buffer_out, sz, kernel_size,
                        sigma[0], sigma[1], sigma[2],
                        color_sigma, datatype);
    }

    if (fft)
    {
        swap(buffer_in, buffer_out);
        fft_filter(buffer_in, sz, buffer_out, datatype);
    }
}


#endif // EXPORT_H
