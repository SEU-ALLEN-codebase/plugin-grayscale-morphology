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


//struct GaussianDenoising: public Handler
//{
//    QVector3D sigma = {16, 16, 4}, skew = {0, 0, 4};
//    void parse(const ArgParser& parser)
//    {
//        parser.parse("hp_sigma", sigma);
//        parser.parse("hp_skew", skew);
//        for (auto i: {0, 1, 2})
//            if (sigma[i] <= 0) throw "GaussianDenoising: Sigma must be positive.";
//    }
//    void operator()(MyImage& img) const;
//};


struct GaussianBlurring: public Handler
{
    QVector3D sigma = {1, 1, 1};
    void parse(const ArgParser& parser)
    {
        parser.parse("blur_sigma", sigma);
        for (auto i: {0, 1, 2})
            if (sigma[i] <= 0) throw "GaussianBlur: Sigma must be positive.";
    }
    void operator()(MyImage& img) const;
};


struct DenoisingThreshold: public Handler
{
    QVector3D flare_interval = {8, 8, 2}, ada_interval = {3, 3, 2};
    V3DLONG ada_sampling = 3, flare_sampling = 8;
    float flare_weight = .02, atten_depth = 4;
    bool flare_x = true, flare_y = true;
    void parse(const ArgParser& parser)
    {
        parser.parse("ada_interval", ada_interval);
        parser.parse("ada_sampling", ada_sampling);
        parser.parse("flare_weight", flare_weight);
        parser.parse("flare_interval", flare_interval);
        parser.parse("flare_sampling", flare_sampling);
        parser.parse("attenuation_depth", atten_depth);
        parser.parse("flare_x", flare_x);
        parser.parse("flare_y", flare_y);
        if (flare_sampling <= 0)
            throw "DenoisingThreshold: Sampling number must be positive.";
        if (flare_weight < 0)
            throw "DenoisingThreshold: Flare weight must be >=0.";
        if (ada_sampling <= 0)
            throw "DenoisingThreshold: Sampling number must be positive.";
        if (atten_depth <= 0)
            throw "DenoisingThreshold: Attenuation depth must be positive.";
        for (auto i: {0, 1, 2})
        {
            if (ada_interval[i] <= 0)
                throw "DenoisingThreshold: Sampling interval must be positive.";
            if (flare_interval[i] <= 0)
                throw "DenoisingThreshold: Sampling interval must be positive.";
        }
    }
    void operator()(MyImage& img) const;
};


//struct MedianFilter: public Handler
//{
//    QVector3D radius = {1, 1, 1};

//    void parse(const ArgParser& parser)
//    {
//        parser.parse("sort_radius", radius);
//        for (auto i: {0, 1, 2})
//            if (radius[i] < 0) throw "SortFilter: Window radius must not be negative.";
//    }
//    void operator()(MyImage& img) const;
//};


//struct MorphoThreshold: public Handler
//{
//    QVector3D win = {2, 2, 1};
//    float thr = 0;
//    void parse(const ArgParser& parser)
//    {
//        parser.parse("morpho_win", win);
//        parser.parse("morpho_thr", thr);
//        for (auto i: {0, 1, 2})
//            if (win[i] <= 0) throw "MorphoFilter: window must be positive.";
//    }
//    void operator()(MyImage& img) const;
//};


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


struct GuoEnhancement: public Handler
{
    QVector3D sigma = {1, 1, 0.3};
    bool bilateral = true, fft = true;
    float color_sigma = 35, gain = 5, cutoff = 25;
    void parse(const ArgParser& parser)
    {
        parser.parse("enh_sigma", sigma);
        for (auto i: {0, 1, 2})
            if (sigma[i] <= 0) throw "GuoEnhancement: sigma must be positive.";
        parser.parse("color_sigma", color_sigma);
        parser.parse("gain", gain);
        parser.parse("cutoff", cutoff);
        parser.parse("fft", fft);
        parser.parse("bilateral", bilateral);
    }
    void operator()(MyImage& img) const;
};


struct NormalEstimationThreshold: public Handler
{
    float bg_pct = .95, norm_qtl = .95;
    void parse(const ArgParser& parser)
    {
        parser.parse("bg_pct", bg_pct);
        if (bg_pct < 0 || bg_pct > 1)
            throw "QuantileThreshold: Percentile must be within 0~1.";
        parser.parse("norm_qtl", norm_qtl);
        if (norm_qtl < 0 || norm_qtl > 1)
            throw "QuantileThreshold: Quantile must be within 0~1.";
    }
    float operator()(const MyImage& img) const;
};


//struct JointThreshold: public Handler
//{
//    float percentile = .99;
//    V3DLONG n_bin_float = 256;
//    void parse(const ArgParser& parser)
//    {
//        parser.parse("fg_percentile", percentile);
//        if (percentile < 0 || percentile > 1)
//            throw "JointThreshold: Percentile must be within 0~1.";
//        parser.parse("n_bin_float", n_bin_float);
//        if (n_bin_float <= 0)
//            throw "JointThreshold: Number of bins must be positive.";
//    }
//    float operator()(const MyImage& img) const;
//};


//struct PercentileThreshold: public Handler
//{
//    float percentile = .99;
//    void parse(const ArgParser& parser)
//    {
//        parser.parse("fg_percentile", percentile);
//        if (percentile < 0 || percentile > 1)
//            throw "SparseAutoThreshold: Percentile must be within 0~1.";
//    }
//    float operator()(const MyImage& img) const;
//};


//struct HistogramEqualization: public Handler
//{
//    float thr = 30;
//    V3DLONG out_datatype = V3D_UINT8, n_bin_float = 256;
//    void parse(const ArgParser& parser)
//    {
//        parser.parse("hist_eq_thr", thr);
//        parser.parse("n_bin_float", n_bin_float);
//        parser.parse("dtype", out_datatype);
//    }
//    float operator()(MyImage& img) const;
//};


struct SomaSearch: public Handler
{
    QVector3D soma_radius = {4, 4, 4}, sigma = {5, 5, 5};
    float bg_thr = 0, z_thickness = 1;
    V3DLONG cnn_type = 3, test_count = 10;
    void parse(const ArgParser& parser)
    {
        parser.parse("soma_radius", soma_radius);
        for (auto i: {0, 1, 2})
            if (soma_radius[i] <= 0)
                throw "SomaSearch: Soma radius must be positive.";
        parser.parse("ms_sigma", sigma);
        for (auto i: {0, 1, 2})
            if (soma_radius[i] <= 0)
                throw "SomaSearch: Sigma must be positive.";
        parser.parse("dt_threshold", bg_thr);
        parser.parse("test_count", test_count);
        parser.parse("z_thickness", z_thickness);
        if (z_thickness <= 0)
            throw "SomaSearch: Z thickness must be positive.";
        parser.parse("connection_type", cnn_type);
        if (cnn_type < 1 || cnn_type > 3)
            throw "SomaSearch: Connection type can only be 1, 2 or 3.";
    }
    void operator()(const MyImage& img, QVector3D& soma) const;
};


#endif // HANDLERS_H
