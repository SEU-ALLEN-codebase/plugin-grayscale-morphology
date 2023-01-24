#ifndef PLUGIN_ACTIONS_H
#define PLUGIN_ACTIONS_H

#include "abstract_actions.h"


class IntegratedAction final: public ImageFilterAnnotationAction
{
public:
    HighPassFilter f1;
    Downsampling f2;
    Enhancement f3;
    SparseAutoThreshold f4;
    ThresholdedHistogramEqualization f5;
    MeanshiftSomaRefinement f6;
    void parse() override {ImageFilterAnnotationAction::_parse({&f1, &f2, &f3, &f4, &f5, &f6});}
    void exec() override;
};


class HighPassFilterAction final: public ImageFilterAction
{
public:
    HighPassFilter f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class DownsamplingAction final: public ImageFilterAction
{
public:
    Downsampling f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class EnhancementAction final: public ImageFilterAction
{
public:
    Enhancement f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class SparseAutoThresholdAction final: public ImageAnnotationAction
{
public:
    SparseAutoThreshold f;
    void parse() override {ImageAnnotationAction::_parse({&f});}
    void exec() override;
};


class ThresholdedHistogramEqualizationAction final: public ImageFilterAnnotationAction
{
    ThresholdedHistogramEqualization f;
    void parse() override {ImageFilterAnnotationAction::_parse({&f});}
    void exec() override;
};


class MeanshiftSomaRefinementAction final: public ImageAnnotationAction
{
public:
    MeanshiftSomaRefinement f;
    void parse() override {ImageAnnotationAction::_parse({&f});}
    void exec() override;
};


#endif // PLUGIN_ACTIONS_H
