#include "handlers.h"
#include "gaussian_filter.h"
#include "hist_eq.h"
#include "imPreProcess/export.h"
#include "gsdt.h"
#include "thresholding.h"
#include "ada_thr.h"
#include "med_filter.h"
#include "morpho_op.h"

/// MIT License
/// by Zuohan Zhao

using namespace std;


/// the gaussian filtered signal is removed from the original image
/// but like an adaptive threshold the negative values are rectified
void GaussianHighPassFilter::operator()(MyImage& img) const
{
    /// fast skewed gaussian filter
    auto blur = img;
    blur.datatype = V3D_FLOAT32;
    blur.create();
    auto&& radius = sigma * 3;
    V3DLONG win_sz[3] = {max((V3DLONG)radius[0]*2+1, 1l),
                         max((V3DLONG)radius[1]*2+1, 1l),
                         max((V3DLONG)radius[2]*2+1, 1l)};
    auto&& src = img.data1d.data();
    auto&& dst = blur.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8: gaussian_filter((v3d_uint8*)src, (v3d_float32*)dst, img.sz, win_sz, sigma, skew); break;
    case V3D_UINT16: gaussian_filter((v3d_uint16*)src, (v3d_float32*)dst, img.sz, win_sz, sigma, skew); break;
    case V3D_FLOAT32: gaussian_filter((v3d_float32*)src, (v3d_float32*)dst, img.sz, win_sz, sigma, skew); break;
    default: throw "GaussianHighPassFilter: Invalid data type for gaussian filter.";
    }

    /// remove low pass signal from the original image.
    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
    for (V3DLONG i = 0; i < tot_sz; ++i)
        img.saturate_set(i, img.at(i) - blur.at(i));
}


void AdaThreshold::operator()(MyImage& img) const
{
    auto out = img;
    out.create();
    auto&& src = img.data1d.data();
    auto&& dst = out.data1d.data();
    V3DLONG h[3] = {max((V3DLONG)interval[0], 1l),
                    max((V3DLONG)interval[1], 1l),
                    max((V3DLONG)interval[2], 1l)};
    switch (img.datatype)
    {
    case V3D_UINT8: ada_thr((v3d_uint8*)src, (v3d_uint8*)dst, img.sz, h, sampling); break;
    case V3D_UINT16: ada_thr((v3d_uint16*)src, (v3d_uint16*)dst, img.sz, h, sampling); break;
    case V3D_FLOAT32: ada_thr((v3d_float32*)src, (v3d_float32*)dst, img.sz, h, sampling); break;
    default: throw "AdaThreshold: Invalid data type for gaussian filter.";
    }
    img = out;
}


void SortFilter::operator()(MyImage& img) const
{
    /// crop
    auto out = img;
    out.create();
    auto&& src = img.data1d.data();
    auto&& dst = out.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8: median_filter((v3d_uint8*)src, (v3d_uint8*)dst, img.sz, radius, order); break;
    case V3D_UINT16: median_filter((v3d_uint16*)src, (v3d_uint16*)dst, img.sz, radius, order); break;
    case V3D_FLOAT32: median_filter((v3d_float32*)src, (v3d_float32*)dst, img.sz, radius, order); break;
    default: throw "SortFilter: Invalid data type.";
    }
    img = out;
}


void MorphoThreshold::operator()(MyImage& img) const
{
    /// crop
    auto&& src = img.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8: morpho_open((v3d_uint8*)src, img.sz, win, thr); break;
    case V3D_UINT16: morpho_open((v3d_uint16*)src, img.sz, win, thr); break;
    case V3D_FLOAT32: morpho_open((v3d_float32*)src, img.sz, win, thr); break;
    default: throw "MedianFilter: Invalid data type.";
    }
}


/// downsampling to remove breakup and make the image isotropic
void Downsampling::operator()(MyImage& img) const
{
    auto out = img;
    out.sz[0] /= scale[0];
    out.sz[1] /= scale[1];
    out.sz[2] /= scale[2];
    auto&& vol = scale[0] * scale[1] * scale[2];
    out.create();
    for (V3DLONG x = 0; x < out.sz[0]; ++x)
        for (V3DLONG y = 0; y < out.sz[1]; ++y)
            for (V3DLONG z = 0; z < out.sz[2]; ++z)
            {
                float tmp = 0;
                for (V3DLONG xx = 0; xx < scale[0]; ++xx)
                    for (V3DLONG yy = 0; yy < scale[1]; ++yy)
                        for (V3DLONG zz = 0; zz < scale[2]; ++zz)
                            tmp = max(tmp, img(x * scale[0]+xx, y * scale[1]+yy, z * scale[2]+zz));
                out.set(x, y, z, tmp);
            }
    img = out;
}


/// S.Guo's image enhancement workflow
void Enhancement::operator()(MyImage& img) const
{
    auto out = img;
    out.create();

    auto&& src = img.data1d.data();
    auto&& dst = out.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8:
        enhancement((v3d_uint8*)src, (v3d_uint8*)dst, img.sz, img.datatype, cutoff,
                    gain, color_sigma, sigma, bilateral, fft);
        break;
    case V3D_UINT16:
        enhancement((v3d_uint16*)src, (v3d_uint16*)dst, img.sz, img.datatype, cutoff,
                    gain, color_sigma, sigma, bilateral, fft);
        break;
    default:
        throw "Enhancement: Data type not allowed.";
    }

    img = out;
}


/// integrate percentile threshold and triangle threshold for a neuron image.
/// triangle use the percentile's result as the start point, if it's greater than the peak.
/// neuron's are of very great & diverse intensity, but their volumes are very thin.
/// this auto thr can remove the background in an explanable and controllable manner.
float SparseAutoThreshold::operator()(const MyImage& img) const
{
    float thr;
    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
    auto&& src = img.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8:
        thr = triangle_threshold((v3d_uint8*)src, tot_sz, 0,
                                 percentile_threshold((v3d_uint8*)src, tot_sz, percentile));
        break;
    case V3D_UINT16:
        thr = triangle_threshold((v3d_uint16*)src, tot_sz, 0,
                                 percentile_threshold((v3d_uint16*)src, tot_sz, percentile));
        break;
    case V3D_FLOAT32:
        thr = triangle_threshold((v3d_float32*)src, tot_sz, n_bin_float,
                                 percentile_threshold((v3d_float32*)src, tot_sz, percentile));
        break;
    default: throw "PercentileEqualization: Datatype not allowed.";
    }
    return thr;
}


float ThresholdedHistogramEqualization::operator()(MyImage& img) const
{
    auto out = img;
    out.datatype = out_datatype;
    out.create();
    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
    auto&& src = img.data1d.data();
    float fct;
    switch (img.datatype)
    {
    case V3D_UINT8: thr_hist_eq((v3d_uint8*)src, tot_sz, 0, thr); break;
    case V3D_UINT16: thr_hist_eq((v3d_uint16*)src, tot_sz, 0, thr); break;
    case V3D_FLOAT32: thr_hist_eq((v3d_float32*)src, tot_sz, n_bin_float, thr); break;
    default: throw "ThresholdedHistogramEqualization: Datatype not allowed.";
    }
    switch (out.datatype)
    {
    case V3D_UINT8: fct = 255; break;
    case V3D_UINT16: fct = 65535; break;
    case V3D_FLOAT32: fct = 1; break;
    default: throw "ThresholdedHistogramEqualization: Datatype not allowed.";
    }
    float max = img.at(0), min = img.at(0);
    for (V3DLONG i = 0; i < tot_sz; ++i)
    {
        max = std::max(img.at(i), max);
        min = std::min(img.at(i), min);
    }
    fct /= max - min;
    for (V3DLONG i = 0; i < tot_sz; ++i)
        out.saturate_set(i, (img.at(i) - min) * fct);
    img = out;
    return (thr - min) * fct;
}


/// Modified from v3d_plugin mean_shift_center by Yujie Li
/// use a 3D gaussian window so that it tends to center at local maxima
QVector3D MeanshiftSomaRefinement::operator()(const MyImage& img) const
{
    QVector3D init(start_pos);
    if (normalized_start)
        init *= QVector3D(img.sz[0], img.sz[1], img.sz[2]);
    V3DLONG win_radius[3] = {min(lround(this->win_radius[0]), img.sz[0]/2),
                             min(lround(this->win_radius[1]), img.sz[1]/2),
                             min(lround(this->win_radius[2]), img.sz[2]/2)};
    V3DLONG start[3] = {lround(init[0]),
                        lround(init[1]),
                        lround(init[2])};

    /// crop
    auto crop = img;
    for (auto i: {0, 1, 2})
        crop.sz[i] = win_radius[i] * 2 + 1;
    crop.create();
    auto xs = max(0l, start[0] - win_radius[0]);
    auto ys = max(0l, start[1] - win_radius[1]);
    auto zs = max(0l, start[2] - win_radius[2]);
    auto xe = min(start[0] + win_radius[0] + 1, img.sz[0]);
    auto ye = min(start[1] + win_radius[1] + 1, img.sz[1]);
    auto ze = min(start[2] + win_radius[2] + 1, img.sz[2]);

    for (V3DLONG x = xs; x < xe; ++x)
        for (V3DLONG y = ys; y < ye; ++y)
            for (V3DLONG z = zs; z < ze; ++z)
                crop.set(x - xs, y - ys, z - zs, img.at(x, y, z));

    /// distance transform
    if (gsdt)
    {
        auto gsdt = crop;
        gsdt.create();
        auto&& src = crop.data1d.data();
        auto&& dst = gsdt.data1d.data();
        switch (crop.datatype)
        {
        case V3D_UINT8:
            fastmarching_dt((v3d_uint8*)src, (v3d_uint8*)dst, crop.sz, cnn_type, bg_thr, z_thickness);
            break;
        case V3D_UINT16:
            fastmarching_dt((v3d_uint16*)src, (v3d_uint16*)dst, crop.sz, cnn_type, bg_thr, z_thickness);
            break;
        case V3D_FLOAT32:
            fastmarching_dt((v3d_float32*)src, (v3d_float32*)dst, crop.sz, cnn_type, bg_thr, z_thickness);
            break;
        default:
            throw "MeanshiftSomaRefinement: Invalid data type.";
        }
        crop = gsdt;
    }

    /// meanshift
    auto&& center = init - QVector3D(xs, ys, zs);
    V3DLONG count = 0;
    float center_dist = 1;
    auto&& search_radius = sigma * 3;
    auto&& s2 = sigma * sigma;
    while (center_dist >= .5 && count < test_count)
    {
        float weight = 0;
        QVector3D total = {0, 0, 0};
        ++count;

        auto&& start = center - search_radius;
        auto&& end = center + search_radius;

        auto xs = max(0l, lround(start[0]));
        auto ys = max(0l, lround(start[1]));
        auto zs = max(0l, lround(start[2]));
        auto xe = min(lround(end[0]) + 1, crop.sz[0]);
        auto ye = min(lround(end[1]) + 1, crop.sz[1]);
        auto ze = min(lround(end[2]) + 1, crop.sz[2]);

        for(V3DLONG x = xs; x < xe; ++x)
            for(V3DLONG y = ys; y < ye; ++y)
                for(V3DLONG z = zs; z < ze; ++z)
                {
                    QVector3D coord(x, y, z);
                    auto&& shift = coord - center;
                    if ((shift / search_radius).lengthSquared() > 1)
                        continue;
                    auto&& w = exp(QVector3D::dotProduct(shift / s2, {.5, .5, .5})) * crop.at(x, y, z);
                    total += w * coord;
                    weight += w;
                }
        if (total.length() < 1e-5)
        {
            qDebug() << "Sphere surrounding the marker is zero. Mean-shift cannot happen. Marker location will not move.";
            return init;
        }
        auto&& new_center = total / weight;
        center_dist = (new_center - center).length();
        center = new_center;
    }
    return center + QVector3D(xs, ys, zs);
}
