#ifndef GAUSSIAN_FILTER_H
#define GAUSSIAN_FILTER_H

#include <v3d_interface.h>
#include <QVector3D>
#include "stats.hpp"

/// MIT license
/// by Zuohan Zhao
/// gaussian filter modified from v3d plugin gausianfilter by Yang Yu
/// the gaussian filter isn't weight normalized


template <class T>
void gaussian_filter(T* data1d,
                     v3d_float32* dst,
                     V3DLONG in_sz[4],
                     const V3DLONG win_sz[3],
                     const QVector3D& sigma,
                     const QVector3D& skew)
{
    if (!data1d || !in_sz || in_sz[0]<=0 || in_sz[1]<=0 || in_sz[2]<=0 || in_sz[3]<=0 || !dst)
        throw "High pass filter: Invalid parameters to gaussian_filter.";

    auto&& pagesz = in_sz[0] * in_sz[1] * in_sz[2];

    /// copy
    for(V3DLONG i = 0; i < pagesz; ++i) dst[i] = data1d[i];

    /// filter
    /// iterate through x, y, z
    for (auto dim: {0, 1, 2})
    {
        if (in_sz[dim] < 2)
            continue;

        /// allocate skewed gaussian distribution 1D
        QSharedPointer<float> weights_buffer(new float[win_sz[dim]], std::default_delete<float[]>());
        if (weights_buffer.isNull())
            throw "High pass filter: Memory allocation failed in gaussian filter.";
        auto&& weights = weights_buffer.data();
        auto&& half = (win_sz[dim]-1) / 2;
        for (V3DLONG i = 0; i <= half; ++i)
        {
            auto&& j = i - half;
            weights[i] = weights[win_sz[dim]-i-1] =
                    2 * stats::dnorm(j, 0, sigma[dim]) * stats::pnorm(skew[dim] * j, 0, sigma[dim]);
        }

        /// temp buffer of raw image in 1D
        /// out-of-range parts are reflected padding
        QSharedPointer<float> extension_buffer(new float[in_sz[dim] + win_sz[dim] * 2], std::default_delete<float[]>());
        if (extension_buffer.isNull())
            throw "High pass filter: Memory allocation failed in gaussian filter.";
        auto&& extension = extension_buffer.data();

        auto&& offset = win_sz[dim] / 2;
        auto&& extStop = extension + in_sz[dim] + offset;
        auto&& dim1 = (dim + 1) % 3;
        auto&& dim2 = (dim + 2) % 3;
        auto index = [&](QList<V3DLONG> ind) {
            return in_sz[0]*in_sz[1]*ind[dim] + in_sz[0]*ind[dim1] + ind[dim2];
        };

        for (V3DLONG i = 0; i < in_sz[dim2]; ++i)
            for (V3DLONG j = 0; j < in_sz[dim1]; ++j)
            {
                auto extIter = extension + win_sz[dim];
                for(V3DLONG k = 0; k < in_sz[dim]; ++k)
                    *(extIter++) = dst[index({i, j, k})];

                ///   Extend image
                auto&& stop_line = extension - 1;
                auto extLeft = extension + win_sz[dim] - 1;
                auto arrLeft = extLeft + 2;
                auto extRight = extLeft + in_sz[dim] + 1;
                auto arrRight = extRight - 2;

                while (extLeft > stop_line)
                {
                    *(extLeft--) = *(arrLeft++);
                    *(extRight++) = *(arrRight--);

                }
                ///	Filtering
                extIter = extension + offset;
                auto resIter = dst + index({i, j, 0});
                while (extIter < extStop)
                {
                    double sum = 0.;
                    auto weightIter = weights;
                    auto&& End = weights + win_sz[dim];
                    auto arrIter = extIter;
                    while (weightIter < End)
                        sum += *(weightIter++) * float(*(arrIter++));
                    extIter++;
                    *(resIter++) = sum;

                }
            }
    }
}


#endif // GAUSSIAN_FILTER_H
