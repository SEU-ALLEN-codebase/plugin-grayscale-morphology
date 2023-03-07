#include "plugin_actions.h"
#include "blur_dialog.h"
#include "denoise_dialog.h"
#include "downsample_dialog.h"
#include "integrated_filter_dialog.h"
#include "utils.h"


void IntegratedFilterAction::exec()
{
    _step("1. BACKGROUND AND FLARE DENOISE");
    f1(img);
    _step("2. GAUSSIAN BLUR");
    f2(img);
    _step("3. DOWNSAMPLE");
    f3(img);
}


void IntegratedFilterAction::dialog()
{
    auto curwin = callback->currentImageWindow();
    if (!curwin)
        throw "You don't have any image opened in the main window.";
    auto p4DImage = callback->getImage(curwin);
    if (!p4DImage)
        throw "The image pointer is invalid. Ensure your data is valid and try again!";
    if (p4DImage->getCDim() > 1)
        throw "Only accept grayscale images.";
    IntegratedFilterDialog dialog(parent);
    if (dialog.exec() != QDialog::Accepted)
        return;
    dialog.update();
    dialog.setParams(f1, f2, f3);
    img.assign(*p4DImage);
    exec();
    auto&& tot_bytes = img.sz[0] * img.sz[1] * img.sz[2] * img.sz[3] * img.datatype;
    auto newbyte = new uchar[tot_bytes];
    if (!newbyte)
        throw "Memory allocation error for a new window of image.";
    memcpy(newbyte, img.data1d.data(), tot_bytes);
    p4DImage = new Image4DSimple();
    p4DImage->setData(newbyte, img.sz[0], img.sz[1], img.sz[2], img.sz[3], img.datatype);
    if (!dialog.overwrite())
    {
        curwin = callback->newImageWindow();
        callback->setImageName(curwin, "Filtered Image");
    }
    callback->setImage(curwin, p4DImage);
    callback->updateImageWindow(curwin);
}


void BlurAction::exec()
{
    _step("GAUSSIAN BLUR");
    f(img);
    _flag_end();
}


void BlurAction::dialog()
{
    auto curwin = callback->currentImageWindow();
    if (!curwin)
        throw "You don't have any image opened in the main window.";
    auto p4DImage = callback->getImage(curwin);
    if (!p4DImage)
        throw "The image pointer is invalid. Ensure your data is valid and try again!";
    if (p4DImage->getCDim() > 1)
        throw "Only accept grayscale images.";
    BlurDialog dialog(parent);
    if (dialog.exec() != QDialog::Accepted)
        return;
    dialog.update();
    dialog.setParams(f);
    img.assign(*p4DImage);
    exec();
    if (dialog.overwrite())
        memcpy(p4DImage->getRawData(), img.data1d.data(), p4DImage->getTotalBytes());
    else
    {
        auto newbyte = new uchar[p4DImage->getTotalBytes()];
        if (!newbyte)
            throw "Memory allocation error for a new window of image.";
        memcpy(newbyte, img.data1d.data(), p4DImage->getTotalBytes());
        p4DImage = new Image4DSimple();
        p4DImage->setData(newbyte, img.sz[0], img.sz[1], img.sz[2], img.sz[3], img.datatype);
        curwin = callback->newImageWindow();
        callback->setImageName(curwin, "Blurred Image");
        callback->setImage(curwin, p4DImage);
    }
    callback->updateImageWindow(curwin);
}


void DenoiseAction::exec()
{
    _step("BACKGROUND AND FLARE DENOISE");
    f(img);
}

void DenoiseAction::dialog()
{
    auto curwin = callback->currentImageWindow();
    if (!curwin)
        throw "You don't have any image opened in the main window.";
    auto p4DImage = callback->getImage(curwin);
    if (!p4DImage)
        throw "The image pointer is invalid. Ensure your data is valid and try again!";
    if (p4DImage->getCDim() > 1)
        throw "Only accept grayscale images.";
    DenoiseDialog dialog(parent);
    if (dialog.exec() != QDialog::Accepted)
        return;
    dialog.update();
    dialog.setParams(f);
    img.assign(*p4DImage);
    exec();
    if (dialog.overwrite())
        memcpy(p4DImage->getRawData(), img.data1d.data(), p4DImage->getTotalBytes());
    else
    {
        auto newbyte = new uchar[p4DImage->getTotalBytes()];
        if (!newbyte)
            throw "Memory allocation error for a new window of image.";
        memcpy(newbyte, img.data1d.data(), p4DImage->getTotalBytes());
        p4DImage = new Image4DSimple();
        p4DImage->setData(newbyte, img.sz[0], img.sz[1], img.sz[2], img.sz[3], img.datatype);
        curwin = callback->newImageWindow();
        callback->setImageName(curwin, "Denoised Image");
        callback->setImage(curwin, p4DImage);
    }
    callback->updateImageWindow(curwin);
}

void DownsampleAction::exec()
{
    _step("DOWNSAMPLE");
    f(img);
}


void DownsampleAction::dialog()
{
    auto curwin = callback->currentImageWindow();
    if (!curwin)
        throw "You don't have any image opened in the main window.";
    auto p4DImage = callback->getImage(curwin);
    if (!p4DImage)
        throw "The image pointer is invalid. Ensure your data is valid and try again!";
    if (p4DImage->getCDim() > 1)
        throw "Only accept grayscale images.";
    DownsampleDialog dialog(parent);
    if (dialog.exec() != QDialog::Accepted)
        return;
    dialog.update();
    dialog.setParams(f);
    img.assign(*p4DImage);
    exec();
    auto&& tot_bytes = img.sz[0] * img.sz[1] * img.sz[2] * img.sz[3] * img.datatype;
    auto newbyte = new uchar[tot_bytes];
    if (!newbyte)
        throw "Memory allocation error for a new window of image.";
    memcpy(newbyte, img.data1d.data(), tot_bytes);
    p4DImage = new Image4DSimple();
    p4DImage->setData(newbyte, img.sz[0], img.sz[1], img.sz[2], img.sz[3], img.datatype);
    if (!dialog.overwrite())
    {
        curwin = callback->newImageWindow();
        callback->setImageName(curwin, "Downsampled Image");
    }
    callback->setImage(curwin, p4DImage);
    callback->updateImageWindow(curwin);
}


void GuoEnhAction::exec()
{
    _step("IMAGE ENHANCE");
    f(img);
}


void SomaSearchAction::exec()
{
    _step("SOMA SEARCH");
    for (auto i = marker_list.begin(); i != marker_list.end(); ++i)
    {
        QVector3D soma(i->x, i->y, i->z);
        f(img, soma);
        i->x = soma[0];
        i->y = soma[1];
        i->z = soma[2];
    }
}
