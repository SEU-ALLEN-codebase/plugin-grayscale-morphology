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
void gaussian_filter_skew(const T* src,
                          v3d_float32* const dst,
                          const V3DLONG in_sz[4],
                          const V3DLONG win_sz[3],
                          const QVector3D& sigma,
                          const QVector3D& skew = {0, 0, 0})
{
    if (!src || !in_sz || in_sz[0]<=0 || in_sz[1]<=0 || in_sz[2]<=0 || in_sz[3]<=0 || !dst)
        throw "Gaussian Filter: Invalid parameters to gaussian_filter.";

    // copy
    {
        const auto pagesz = in_sz[0] * in_sz[1] * in_sz[2];
        for (V3DLONG i = 0; i < pagesz; ++i) dst[i] = src[i];
    }

    // filter
    // iterate through x, y, z
    V3DLONG sz[5] = {in_sz[0], in_sz[1], in_sz[2], in_sz[0], in_sz[1]};
    V3DLONG stride[5] = {1, in_sz[0], in_sz[0]*in_sz[1], 1, in_sz[0]};
    for (auto dim: {0, 1, 2})
    {
        if (sz[dim] < 2)
            continue;

        // allocate skewed gaussian distribution 1D
        QSharedPointer<float> weights_buffer(new float[win_sz[dim]], std::default_delete<float[]>());
        if (weights_buffer.isNull())
            throw "Gaussian Filter: Memory allocation failed in gaussian filter.";
        const auto weights = weights_buffer.data();
        {
            const auto half = (win_sz[dim] - 1) / 2;
            for (V3DLONG i = 0; i <= half; ++i)
                weights[i] = weights[win_sz[dim]-i-1] = stats::dnorm(i - half, 0, sigma[dim]);
            float k = 0;
            for (V3DLONG i = 0; i < win_sz[dim]; ++i)
            {
                weights[i] *= stats::pnorm(skew[dim] * (i - half), 0, sigma[dim]);
                k += weights[i];
            }
            if (dim == 0)
                printf("X direction\n");
            else if (dim == 1)
                printf("Y direction\n");
            else
                printf("Z direction\n");
            for (V3DLONG i = 0; i < win_sz[dim]; ++i)
            {
                weights[i] /= k;
                printf("%f\t", weights[i]);
                if ((i + 1) % 5 == 0 && i < win_sz[dim] - 1) printf("\n");
            }
            printf("\n");
        }

        // temp buffer of raw image in 1D
        // out-of-range parts are reflected padding
        QSharedPointer<float> extension_buffer(new float[sz[dim] + win_sz[dim] * 2], std::default_delete<float[]>());
        if (extension_buffer.isNull())
            throw "Gaussian Filter: Memory allocation failed in gaussian filter.";
        const auto extension = extension_buffer.data();
        const auto offset = win_sz[dim] / 2;
        const auto ext_stop = extension + sz[dim] + offset;

        for (V3DLONG i = 0, buf_start1 = 0;
             i < sz[dim + 2];
             ++i, buf_start1 += stride[dim + 2])
        {
            for (V3DLONG j = 0, buf_start2 = buf_start1;
                 j < sz[dim + 1];
                 ++j, buf_start2 += stride[dim + 1])
            {
                // copy the buffer along the dim
                {
                    auto&& ext_ptr = extension + win_sz[dim];
                    auto&& res_ptr = dst + buf_start2;
                    for(V3DLONG k = 0; k < sz[dim]; ++k)
                    {
                        *(ext_ptr++) = *res_ptr;
                        res_ptr += stride[dim];
                    }
                }

                // extend image
                {
                    auto&& ext_l = extension + win_sz[dim] - 1;
                    auto&& arr_l = ext_l + 2;
                    auto&& ext_r = ext_l + sz[dim] + 1;
                    auto&& arr_r = ext_r - 2;
                    while (ext_l >= extension)
                    {
                        *(ext_l--) = *(arr_l++);
                        *(ext_r++) = *(arr_r--);
                    }
                }

                //	filtering and copy back
                {
                    auto&& ext_ptr = extension + offset;
                    auto&& res_ptr = dst + buf_start2;
                    while (ext_ptr < ext_stop)
                    {
                        float sum = 0;
                        auto w_ptr = weights;
                        auto arrIter = ext_ptr;
                        auto&& w_end = weights + win_sz[dim];
                        while (w_ptr < w_end)
                            sum += *(w_ptr++) * *(arrIter++);
                        ++ext_ptr;
                        *res_ptr = sum;
                        res_ptr += stride[dim];
                    }
                }
            }
            printf("\r%d %% completed..", ((i + 1)*100) / sz[dim + 2]); fflush(stdout);
        }
        printf("\n");
    }
}


template <class T>
void gaussian_filter_insitu(T* const src,
                             const V3DLONG in_sz[4],
                             const V3DLONG win_sz[3],
                             const QVector3D& sigma)
{
    if (!src || !in_sz || in_sz[0]<=0 || in_sz[1]<=0 || in_sz[2]<=0 || in_sz[3]<=0)
        throw "Gaussian Filter: Invalid parameters to gaussian_filter.";

    // filter
    // iterate through x, y, z
    V3DLONG sz[5] = {in_sz[0], in_sz[1], in_sz[2], in_sz[0], in_sz[1]};
    V3DLONG stride[5] = {1, in_sz[0], in_sz[0]*in_sz[1], 1, in_sz[0]};
    for (auto dim: {0, 1, 2})
    {
        if (sz[dim] < 2)
            continue;

        // allocate gaussian distribution 1D
        QSharedPointer<float> weights_buffer(new float[win_sz[dim]], std::default_delete<float[]>());
        if (weights_buffer.isNull())
            throw "Gaussian Filter: Memory allocation failed in gaussian filter.";
        const auto weights = weights_buffer.data();
        {
            const auto half = (win_sz[dim] - 1) / 2;
            for (V3DLONG i = 0; i <= half; ++i)
                weights[i] = weights[win_sz[dim]-i-1] = stats::dnorm(i - half, 0, sigma[dim]);
            float k = 0;
            for (V3DLONG i = 0; i < win_sz[dim]; ++i)
                k += weights[i];
            if (dim == 0)
                printf("X direction\n");
            else if (dim == 1)
                printf("Y direction\n");
            else
                printf("Z direction\n");
            for (V3DLONG i = 0; i < win_sz[dim]; ++i)
            {
                weights[i] /= k;
                printf("%f\t", weights[i]);
                if ((i + 1) % 5 == 0 && i < win_sz[dim] - 1) printf("\n");
            }
            printf("\n");
        }

        // temp buffer of raw image in 1D
        // out-of-range parts are reflected padding
        QSharedPointer<float> extension_buffer(new float[sz[dim] + win_sz[dim] * 2], std::default_delete<float[]>());
        if (extension_buffer.isNull())
            throw "Gaussian Filter: Memory allocation failed in gaussian filter.";
        const auto extension = extension_buffer.data();
        const auto offset = win_sz[dim] / 2;
        const auto ext_stop = extension + sz[dim] + offset;

        for (V3DLONG i = 0, buf_start1 = 0;
             i < sz[dim + 2];
             ++i, buf_start1 += stride[dim + 2])
        {
            for (V3DLONG j = 0, buf_start2 = buf_start1;
                 j < sz[dim + 1];
                 ++j, buf_start2 += stride[dim + 1])
            {
                // copy the buffer along the dim
                {
                    auto&& ext_ptr = extension + win_sz[dim];
                    auto&& res_ptr = src + buf_start2;
                    for(V3DLONG k = 0; k < sz[dim]; ++k)
                    {
                        *(ext_ptr++) = *res_ptr;
                        res_ptr += stride[dim];
                    }
                }

                // extend image
                {
                    auto&& ext_l = extension + win_sz[dim] - 1;
                    auto&& arr_l = ext_l + 2;
                    auto&& ext_r = ext_l + sz[dim] + 1;
                    auto&& arr_r = ext_r - 2;
                    while (ext_l >= extension)
                    {
                        *(ext_l--) = *(arr_l++);
                        *(ext_r++) = *(arr_r--);
                    }
                }

                //	filtering and copy back
                {
                    auto&& ext_ptr = extension + offset;
                    auto&& res_ptr = src + buf_start2;
                    while (ext_ptr < ext_stop)
                    {
                        float sum = 0;
                        auto w_ptr = weights;
                        auto arrIter = ext_ptr;
                        auto&& w_end = weights + win_sz[dim];
                        while (w_ptr < w_end)
                            sum += *(w_ptr++) * *(arrIter++);
                        ++ext_ptr;
                        *res_ptr = sum;
                        res_ptr += stride[dim];
                    }
                }
            }
            printf("\r%d %% completed..", ((i + 1)*100) / sz[dim + 2]); fflush(stdout);
        }
        printf("\n");
    }
}


#endif // GAUSSIAN_FILTER_H
