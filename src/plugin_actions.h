#ifndef PLUGIN_ACTIONS_H
#define PLUGIN_ACTIONS_H

#include "abstract_actions.h"


class IntegratedAction final: public ImageFilterAnnotationAction
{
public:
    DenoisingThreshold f1;
    GaussianBlurring f2;
    Downsampling f3;
    NormalEstimationThreshold f4;
    MeanshiftSomaRefinement f5;
    void parse() override {ImageFilterAnnotationAction::_parse({&f1, &f2, &f3, &f4, &f5});}
    void exec() override;
};


class BlurAction final: public ImageFilterAction
{
public:
    GaussianBlurring f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class DenoiseAction final: public ImageFilterAction
{
public:
    DenoisingThreshold f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class DownsampleAction final: public ImageFilterAction
{
public:
    Downsampling f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class GuoEnhAction final: public ImageFilterAction
{
public:
    GuoEnhancement f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class AutoThrAction final: public ImageAnnotationAction
{
public:
    NormalEstimationThreshold f;
    void parse() override {ImageAnnotationAction::_parse({&f});}
    void exec() override;
};


class SomaRefineAction final: public ImageAnnotationAction
{
public:
    MeanshiftSomaRefinement f;
    void parse() override {ImageAnnotationAction::_parse({&f});}
    void exec() override;
};


#endif // PLUGIN_ACTIONS_H
