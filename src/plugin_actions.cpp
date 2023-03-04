#include "plugin_actions.h"
#include "utils.h"


void IntegratedAction::exec()
{
    _flag_start("NEURON IMAGE DENOISE");
    parse();
    load();
    _step("1. BACKGROUND AND FLARE DENOISE");
    f1(img);
    _step("2. GAUSSIAN BLUR");
    f2(img);
    _step("3. DOWNSAMPLE");
    f3(img);
    _step("4. MEANSHIFT SOMA REFINE");
    const auto& soma = f4(img);
    marker_list.clear();
    marker_list.append({soma[0], soma[1], soma[2]});
    marker_list.back().name = "soma";
    save();
    _flag_end();
}


void BlurAction::exec()
{
    _flag_start("NEURON IMAGE DENOISE");
    parse();
    load();
    _step("GAUSSIAN BLUR");
    f(img);
    save();
    _flag_end();
}


void DenoiseAction::exec()
{
    _flag_start("NEURON IMAGE DENOISE");
    parse();
    load();
    _step("BACKGROUND AND FLARE DENOISE");
    f(img);
    save();
    _flag_end();
}


void DownsampleAction::exec()
{
    _flag_start("NEURON IMAGE DENOISE");
    parse();
    load();
    _step("DOWNSAMPLE");
    f(img);
    save();
    _flag_end();
}


void GuoEnhAction::exec()
{
    _flag_start("");
    parse();
    load();
    _step("IMAGE ENHANCE");
    f(img);
    save();
    _flag_end();
}


void SomaRefineAction::exec()
{
    _flag_start("NEURON IMAGE DENOISE");
    parse();
    load();
    _step("MEANSHIFT SOMA REFINE");
    auto&& soma = f(img);
    marker_list.clear();
    marker_list.append({soma[0], soma[1], soma[2]});
    marker_list.back().name = "soma";
    save();
    _flag_end();
}
