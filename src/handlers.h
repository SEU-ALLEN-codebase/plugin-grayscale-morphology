#ifndef HANDLERS_H
#define HANDLERS_H

#include "my_image.h"
#include "argparser.h"
#include <QVector3D>

/// MIT License
/// by Zuohan Zhao
/// all direct computation functions used by the plugin
/// managed by structs of unified API


struct Handler
{
    virtual void parse(const ArgParser& parser) = 0;
};


struct HighPassFilter: public Handler
{
    QVector3D sigma = {8, 8, 2}, skew = {0, 0, -1};
    void parse(const ArgParser& parser)
    {
        parser.parse("hp_sigma", sigma);
        parser.parse("hp_skew", skew);
        for (auto i: {0, 1, 2})
            if (sigma[i] <= 0) throw "HighPassFilter: Sigma must be positive.";
    }
    void operator()(MyImage& img) const;
};


struct Downsampling: public Handler
{
    QVector3D scale = {4, 4, 1};
    void parse(const ArgParser& parser)
    {
        parser.parse("scale", scale);
        for (auto i: {0, 1, 2})
            if (scale[i] <= 0) throw "Downsampling: Scale must be positive";
    }
    void operator()(MyImage& img) const;
};


struct Enhancement: public Handler
{
    QVector3D sigma = {1, 1, 1};
    bool bilateral = true, fft = true;
    float color_sigma = 35, gain = 5, cutoff = 25;
    void parse(const ArgParser& parser)
    {
        parser.parse("enh_sigma", sigma);
        for (auto i: {0, 1, 2})
            if (sigma[i] <= 0) throw "Enhancement: sigma must be positive.";
        parser.parse("color_sigma", color_sigma);
        parser.parse("gain", gain);
        parser.parse("cutoff", cutoff);
        parser.parse("fft", fft);
        parser.parse("bilateral", bilateral);
    }
    void operator()(MyImage& img) const;
};


struct SparseAutoThreshold: public Handler
{
    float percentile = .01;
    V3DLONG n_bin_float = 256;
    void parse(const ArgParser& parser)
    {
        parser.parse("fg_percentile", percentile);
        if (percentile < 0 || percentile > 1)
            throw "SparseAutoThreshold: Percentile must be within 0~1.";
        parser.parse("n_bin_float", n_bin_float);
        if (n_bin_float <= 0)
            throw "SparseAutoThreshold: Number of bins must be positive.";
    }
    float operator()(const MyImage& img) const;
};


struct ThresholdedHistogramEqualization: public Handler
{
    float thr = 0;
    V3DLONG out_datatype = V3D_UINT8, n_bin_float = 256;
    void parse(const ArgParser& parser)
    {
        parser.parse("hist_eq_thr", thr);
        parser.parse("n_bin_float", n_bin_float);
        parser.parse("dtype", out_datatype);
    }
    float operator()(MyImage& img) const;
};


struct MeanshiftSomaRefinement: public Handler
{
    QVector3D start_pos = {.5, .5, .5}, win_radius = {20, 20, 20}, sigma = {3, 3, 3};
    bool normalized_start = true, gsdt = true;
    float bg_thr = 0, z_thickness = 1;
    V3DLONG cnn_type = 3, test_count = 50;
    void parse(const ArgParser& parser)
    {
        parser.parse("normalized_start", normalized_start);
        parser.parse("start_pos", start_pos);
        for (auto i: {0, 1, 2})
            if (start_pos[i] < 0 || start_pos[i] > 1)
                throw "MeanshiftSomaRefinement: Soma start position must be within 0~1.";

        parser.parse("win_radius", win_radius);
        for (auto i: {0, 1, 2})
            if (win_radius[i] <= 0)
                throw "MeanshiftSomaRefinement: Meanshift window radius must be positive.";

        parser.parse("ms_sigma", sigma);
        for (auto i: {0, 1, 2})
            if (sigma[i] <= 0)
                throw "MeanshiftSomaRefinement: Meanshift search radius must be positive.";

        parser.parse("gsdt", gsdt);
        parser.parse("dt_threshold", bg_thr);
        parser.parse("test_count", test_count);
        parser.parse("z_thickness", z_thickness);
        if (z_thickness <= 0)
            throw "MeanshiftSomaRefinement: Z thickness must be positive.";
        parser.parse("connection_type", cnn_type);
        if (cnn_type < 1 || cnn_type > 3)
            throw "MeanshiftSomaRefinement: Connection type can only be 1, 2 or 3.";
    }
    QVector3D operator()(const MyImage& img) const;
};


#endif // HANDLERS_H
