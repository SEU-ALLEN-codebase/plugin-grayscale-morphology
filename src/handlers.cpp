#include "handlers.h"
#include "gaussian_filter.h"
#include "hist_eq.h"
#include "imPreProcess/export.h"
#include "gsdt.h"
#include "thresholding.h"
#include "ada_denoise.h"
#include "med_filter.h"
#include "morpho_op.h"

/// MIT License
/// by Zuohan Zhao

using namespace std;


/// the gaussian filtered signal is removed from the original image
/// but like an adaptive threshold the negative values are rectified
//void GaussianDenoising::operator()(MyImage& img) const
//{
//    /// fast skewed gaussian filter
//    auto blur = img;
//    blur.datatype = V3D_FLOAT32;
//    blur.create();
//    auto&& radius = sigma * 3;
//    V3DLONG win_sz[3] = {max(lround(radius[0])*2+1, 1l),
//                         max(lround(radius[1])*2+1, 1l),
//                         max(lround(radius[2])*2+1, 1l)};
//    auto&& src = img.data1d.data();
//    auto&& dst = blur.data1d.data();
//    switch (img.datatype)
//    {
//    case V3D_UINT8: gaussian_filter_skew((v3d_uint8*)src, (v3d_float32*)dst, img.sz, win_sz, sigma, skew); break;
//    case V3D_UINT16: gaussian_filter_skew((v3d_uint16*)src, (v3d_float32*)dst, img.sz, win_sz, sigma, skew); break;
//    case V3D_FLOAT32: gaussian_filter_skew((v3d_float32*)src, (v3d_float32*)dst, img.sz, win_sz, sigma, skew); break;
//    default: throw "GaussianDenoising: Invalid data type for gaussian filter.";
//    }

//    /// remove low pass signal from the original image.
//    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
//    for (V3DLONG i = 0; i < tot_sz; ++i)
//        img.saturate_set(i, img.at(i) - blur.at(i));
//}



void GaussianBlurring::operator()(MyImage& img) const
{
    /// fast skewed gaussian filter
    auto&& radius = sigma * 3;
    V3DLONG win_sz[3] = {max(lround(radius[0])*2+1, 1l),
                         max(lround(radius[1])*2+1, 1l),
                         max(lround(radius[2])*2+1, 1l)};
    auto&& src = img.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8: gaussian_filter_insitu((v3d_uint8*)src, img.sz, win_sz, sigma); break;
    case V3D_UINT16: gaussian_filter_insitu((v3d_uint16*)src, img.sz, win_sz, sigma); break;
    case V3D_FLOAT32: gaussian_filter_insitu((v3d_float32*)src, img.sz, win_sz, sigma); break;
    default: throw "GaussianBlur: Invalid data type for gaussian filter.";
    }
}


void DenoisingThreshold::operator()(MyImage& img) const
{
    auto out = img;
    out.create();
    auto&& src = img.data1d.data();
    auto&& dst = out.data1d.data();
    V3DLONG h1[3] = {max((V3DLONG)ada_interval[0], 1l),
                     max((V3DLONG)ada_interval[1], 1l),
                     max((V3DLONG)ada_interval[2], 1l)};
    V3DLONG h2[3] = {max((V3DLONG)flare_interval[0], 1l),
                     max((V3DLONG)flare_interval[1], 1l),
                     max((V3DLONG)flare_interval[2], 1l)};
    switch (img.datatype)
    {
    case V3D_UINT8:
        ada_denoise((v3d_uint8*)src, (v3d_uint8*)dst, img.sz, h1, ada_sampling,
                    h2, flare_sampling, flare_weight, atten_depth, flare_x, flare_y);
        break;
    case V3D_UINT16:
        ada_denoise((v3d_uint16*)src, (v3d_uint16*)dst, img.sz, h1, ada_sampling,
                    h2, flare_sampling, flare_weight, atten_depth, flare_x, flare_y);
        break;
    case V3D_FLOAT32:
        ada_denoise((v3d_float32*)src, (v3d_float32*)dst, img.sz, h1, ada_sampling,
                    h2, flare_sampling, flare_weight, atten_depth, flare_x, flare_y);
        break;
    default: throw "DenoisingThreshold: Invalid data type for gaussian filter.";
    }
    img = out;
}


//void MedianFilter::operator()(MyImage& img) const
//{
//    /// crop
//    auto out = img;
//    out.create();
//    auto&& src = img.data1d.data();
//    auto&& dst = out.data1d.data();
//    switch (img.datatype)
//    {
//    case V3D_UINT8: median_filter((v3d_uint8*)src, (v3d_uint8*)dst, img.sz, radius); break;
//    case V3D_UINT16: median_filter((v3d_uint16*)src, (v3d_uint16*)dst, img.sz, radius); break;
//    case V3D_FLOAT32: median_filter((v3d_float32*)src, (v3d_float32*)dst, img.sz, radius); break;
//    default: throw "MedianFilter: Invalid data type.";
//    }
//    img = out;
//}


//void MorphoThreshold::operator()(MyImage& img) const
//{
//    /// crop
//    auto&& src = img.data1d.data();
//    switch (img.datatype)
//    {
//    case V3D_UINT8: morpho_open((v3d_uint8*)src, img.sz, win, thr); break;
//    case V3D_UINT16: morpho_open((v3d_uint16*)src, img.sz, win, thr); break;
//    case V3D_FLOAT32: morpho_open((v3d_float32*)src, img.sz, win, thr); break;
//    default: throw "MedianFilter: Invalid data type.";
//    }
//}


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
void GuoEnhancement::operator()(MyImage& img) const
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
        throw "GuoEnhancement: Invalid datatype.";
    }

    img = out;
}


/// integrate percentile threshold and triangle threshold for a neuron image.
/// triangle use the percentile's result as the start point, if it's greater than the peak.
/// neuron's are of very great & diverse intensity, but their volumes are very thin.
/// this auto thr can remove the background in an explanable and controllable manner.
//float JointThreshold::operator()(const MyImage& img) const
//{
//    float thr;
//    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
//    auto&& src = img.data1d.data();
//    switch (img.datatype)
//    {
//    case V3D_UINT8:
//        thr = triangle_threshold((v3d_uint8*)src, tot_sz, 0,
//                                 percentile_threshold((v3d_uint8*)src, tot_sz, percentile));
//        break;
//    case V3D_UINT16:
//        thr = triangle_threshold((v3d_uint16*)src, tot_sz, 0,
//                                 percentile_threshold((v3d_uint16*)src, tot_sz, percentile));
//        break;
//    case V3D_FLOAT32:
//        thr = triangle_threshold((v3d_float32*)src, tot_sz, n_bin_float,
//                                 percentile_threshold((v3d_float32*)src, tot_sz, percentile));
//        break;
//    default: throw "PercentileEqualization: Datatype not allowed.";
//    }
//    return thr;
//}


/// Estimate a nomral distribution for the histogram of background pixels
/// and use it to calculate a threshold
float NormalEstimationThreshold::operator()(const MyImage& img) const
{
    float thr;
    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
    auto&& src = img.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8:
        thr = percentile_threshold((v3d_uint8*)src, tot_sz, bg_pct);
        thr = normal_threshold((v3d_uint8*)src, tot_sz, norm_qtl, thr);
        break;
    case V3D_UINT16:
        thr = percentile_threshold((v3d_uint16*)src, tot_sz, bg_pct);
        thr = normal_threshold((v3d_uint16*)src, tot_sz, norm_qtl, thr);
        break;
    case V3D_FLOAT32:
        thr = percentile_threshold((v3d_float32*)src, tot_sz, bg_pct);
        thr = normal_threshold((v3d_float32*)src, tot_sz, norm_qtl, thr);
        break;
    default: throw "NormalEstimationThreshold: Invalid datatype.";
    }
    return thr;
}


//float PercentileThreshold::operator()(const MyImage& img) const
//{
//    float thr;
//    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
//    auto&& src = img.data1d.data();
//    switch (img.datatype)
//    {
//    case V3D_UINT8:
//        thr = percentile_threshold((v3d_uint8*)src, tot_sz, percentile);
//        break;
//    case V3D_UINT16:
//        thr = percentile_threshold((v3d_uint16*)src, tot_sz, percentile);
//        break;
//    case V3D_FLOAT32:
//        thr = percentile_threshold((v3d_float32*)src, tot_sz, percentile);
//        break;
//    default: throw "PercentileThreshold: Invalid datatype.";
//    }
//    return thr;
//}


//float HistogramEqualization::operator()(MyImage& img) const
//{
//    auto out = img;
//    out.datatype = out_datatype;
//    out.create();
//    auto&& tot_sz = img.sz[0] * img.sz[1] * img.sz[2];
//    auto&& src = img.data1d.data();
//    float fct;
//    switch (img.datatype)
//    {
//    case V3D_UINT8: thr_hist_eq((v3d_uint8*)src, tot_sz, 0, thr); break;
//    case V3D_UINT16: thr_hist_eq((v3d_uint16*)src, tot_sz, 0, thr); break;
//    case V3D_FLOAT32: thr_hist_eq((v3d_float32*)src, tot_sz, n_bin_float, thr); break;
//    default: throw "HistogramEqualization: Invalid datatype.";
//    }
//    switch (out.datatype)
//    {
//    case V3D_UINT8: fct = 255; break;
//    case V3D_UINT16: fct = 65535; break;
//    case V3D_FLOAT32: fct = 1; break;
//    default: throw "HistogramEqualization: Invalid datatype.";
//    }
//    float max = img.at(0), min = img.at(0);
//    for (V3DLONG i = 0; i < tot_sz; ++i)
//    {
//        max = std::max(img.at(i), max);
//        min = std::min(img.at(i), min);
//    }
//    fct /= max - min;
//    for (V3DLONG i = 0; i < tot_sz; ++i)
//        out.saturate_set(i, (img.at(i) - min) * fct);
//    img = out;
//    return (thr - min) * fct;
//}


void SomaSearch::operator()(const MyImage& img, QVector3D& soma) const
{
    /// distance transform
    auto gsdt = img;
    gsdt.create();
    auto&& src = img.data1d.data();
    auto&& dst = gsdt.data1d.data();
    switch (img.datatype)
    {
    case V3D_UINT8:
        fastmarching_dt((v3d_uint8*)src, (v3d_uint8*)dst, img.sz, cnn_type, bg_thr, z_thickness);
        break;
    case V3D_UINT16:
        fastmarching_dt((v3d_uint16*)src, (v3d_uint16*)dst, img.sz, cnn_type, bg_thr, z_thickness);
        break;
    case V3D_FLOAT32:
        fastmarching_dt((v3d_float32*)src, (v3d_float32*)dst, img.sz, cnn_type, bg_thr, z_thickness);
        break;
    default:
        throw "MeanshiftSomaRefinement: Invalid datatype.";
    }

    V3DLONG count = 0;
    float center_dist = 1;
    while (center_dist >= .5 && count < test_count)
    {
        ++count;
        const auto& start = soma - win_radius;
        const auto& end = soma + win_radius;
        const auto xs = max(0l, lround(start[0]));
        const auto ys = max(0l, lround(start[1]));
        const auto zs = max(0l, lround(start[2]));
        const auto xe = min(lround(end[0]) + 1, gsdt.sz[0]);
        const auto ye = min(lround(end[1]) + 1, gsdt.sz[1]);
        const auto ze = min(lround(end[2]) + 1, gsdt.sz[2]);

        auto new_center = soma;
        for(V3DLONG x = xs; x < xe; ++x)
            for(V3DLONG y = ys; y < ye; ++y)
                for(V3DLONG z = zs; z < ze; ++z)
                {
                    if (gsdt.at(x, y, z) > gsdt.at(new_center[0], new_center[1], new_center[2]))
                        new_center = QVector3D(x, y, z);
                }
        center_dist = (new_center - soma).length();
        soma = new_center;
    }
}
