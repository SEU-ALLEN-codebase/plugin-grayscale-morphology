#ifndef PLUGIN_ACTIONS_H
#define PLUGIN_ACTIONS_H

#include "abstract_actions.h"


class IntegratedFilterAction final: public ImageFilterAction
{
public:
    IntegratedFilterAction()
    {
        _flag_start("NEURON IMAGE DENOISE");
    }
    ~IntegratedFilterAction()
    {
        _flag_end();
    }
    DenoisingThreshold f1;
    GaussianBlurring f2;
    Downsampling f3;
    void parse() override {ImageFilterAction::_parse({&f1, &f2, &f3});}
    void exec() override;
    virtual void dialog();
};


class BlurAction final: public ImageFilterAction
{
public:
    BlurAction()
    {
        _flag_start("NEURON IMAGE DENOISE");
    }
    ~BlurAction()
    {
        _flag_end();
    }
    GaussianBlurring f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
    virtual void dialog();
};


class DenoiseAction final: public ImageFilterAction
{
public:
    DenoiseAction()
    {
        _flag_start("NEURON IMAGE DENOISE");
    }
    ~DenoiseAction()
    {
        _flag_end();
    }
    DenoisingThreshold f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
    virtual void dialog();
};


class DownsampleAction final: public ImageFilterAction
{
public:
    DownsampleAction()
    {
        _flag_start("NEURON IMAGE DENOISE");
    }
    ~DownsampleAction()
    {
        _flag_end();
    }
    Downsampling f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
    virtual void dialog();
};


class GuoEnhAction final: public ImageFilterAction
{
public:
    GuoEnhAction()
    {
        _flag_start("NEURON IMAGE DENOISE");
    }
    ~GuoEnhAction()
    {
        _flag_end();
    }
    GuoEnhancement f;
    void parse() override {ImageFilterAction::_parse({&f});}
    void exec() override;
};


class SomaSearchAction final: public ImageAnnotationAction
{
public:
    SomaSearchAction()
    {
        _flag_start("NEURON IMAGE DENOISE");
    }
    ~SomaSearchAction()
    {
        _flag_end();
    }
    SomaSearch f;
    void parse() override {ImageAnnotationAction::_parse({&f});}
    void exec() override;
};


#endif // PLUGIN_ACTIONS_H
