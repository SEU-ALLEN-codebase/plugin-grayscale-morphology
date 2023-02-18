#include "plugin_actions.h"
#include "utils.h"


void IntegratedAction::exec()
{
    _flag_start("Integrated Neuron Image Processing");
    load();
    qDebug() << "Adaptive Thresholding..";
    f1(img);
//    qDebug() << "Downsampling..";
//    f2(img);
//    qDebug() << "Enhancing..";
//    f3(img);
    qDebug() << "Thresholding..";
    auto&& thr = f2(img);
    qDebug() << "Histogram Equalization..";
    f3.thr = thr;
    thr = f3(img);
    qDebug() << "Sort Filtering..";
    f4(img);
    qDebug() << "Refining soma..";
    f5.bg_thr = thr;
    auto&& soma = f5(img);
    marker_list.clear();
    marker_list.append({soma[0], soma[1], soma[2]});
    marker_list.back().name = "refined_soma_with_thr";
    marker_list.back().comments = std::to_string(lround(thr));
    save();
    _flag_end();
}


void GaussianHighPassFilterAction::exec()
{
    _flag_start("High Pass Filtering");
    load();
    f(img);
    save();
    _flag_end();
}


void MorphoThresholdAction::exec()
{
    _flag_start("Morphological Thresholding");
    load();
    f(img);
    save();
    _flag_end();
}


void SortFilterAction::exec()
{
    _flag_start("Median Filtering");
    load();
    f(img);
    save();
    _flag_end();
}


void AdaThresholdAction::exec()
{
    _flag_start("Adaptive Thresholding");
    load();
    f(img);
    save();
    _flag_end();
}


void DownsamplingAction::exec()
{
    _flag_start("Downsampling");
    load();
    f(img);
    save();
    _flag_end();
}


void EnhancementAction::exec()
{
    _flag_start("Enhancement");
    load();
    f(img);
    save();
    _flag_end();
}


void SparseAutoThresholdAction::exec()
{
    _flag_start("Sparse Auto Thresholding");
    load();
    auto&& thr = f(img);
    marker_list.clear();
    marker_list.append({0, 0, 0});
    marker_list.back().name = "threshold";
    marker_list.back().comments = std::to_string(lround(thr));
    save();
    _flag_end();
}


void ThresholdedHistogramEqualizationAction::exec()
{
    _flag_start("Thresholded Histogram Equalization");
    load();
    auto&& thr = f(img);
    marker_list.clear();
    marker_list.append({0, 0, 0});
    marker_list.back().name = "histeq_threshold";
    marker_list.back().comments = std::to_string(lround(thr));
    save();
    _flag_end();
}


void MeanshiftSomaRefinementAction::exec()
{
    _flag_start("Meanshift Soma Refinement");
    load();
    auto&& soma = f(img);
    marker_list.clear();
    marker_list.append({soma[0], soma[1], soma[2]});
    marker_list.back().name = "refined_soma";
    save();
    _flag_end();
}
