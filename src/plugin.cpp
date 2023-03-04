/* plugin.cpp
 * TODO
 * 2022-12-20 : by Zuohan
 */
 
#include "plugin.h"
#include "plugin_actions.h"
#include <QMessageBox>

using namespace std;


QStringList NeuronImageDenoise::menulist() const
{
    return QStringList()
            << tr("about");
}


QStringList NeuronImageDenoise::funclist() const
{
    return QStringList()
            << tr("integrated")
            << tr("denoise")
            << tr("blur")
            << tr("max_ds")
            << tr("refine_soma")
            << tr("guo_enh")
            << tr("help");
}


void NeuronImageDenoise::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    try
    {
        QSharedPointer<BaseAction> action;

        if (!action.isNull())
        {
            action->init(callback, parent);
            action->dialog();
            action->exec();
        }
        else
            QMessageBox::about(0, "Neuron Image Denoise", "TODO");
    }
    catch(const QString& msg)
    {
        QMessageBox::critical(0, "Error", msg);
    }
    catch(...)
    {
        QMessageBox::critical(0, "Error", "Some error ocurred during execution.");
    }
}


bool NeuronImageDenoise::dofunc(const QString & func_name, const V3DPluginArgList & input,
                                V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
    bool flag = true;
    try
    {
        QSharedPointer<BaseAction> action;
        bool command = true;

        if (func_name == tr("integrated"))
            action = QSharedPointer<BaseAction>(new IntegratedAction);
        else if (func_name == tr("denoise"))
            action = QSharedPointer<BaseAction>(new DenoiseAction);
        else if (func_name == tr("blur"))
            action = QSharedPointer<BaseAction>(new BlurAction);
        else if (func_name == tr("max_ds"))
            action = QSharedPointer<BaseAction>(new DownsampleAction);
        else if (func_name == tr("refine_soma"))
            action = QSharedPointer<BaseAction>(new SomaRefineAction);
        else if (func_name == tr("guo_enh"))
            action = QSharedPointer<BaseAction>(new GuoEnhAction);
        else
            command = false;

        if (!action.isNull())
        {
            action->init(input, output, callback, parent);
            action->exec();
        }
        else if (command)
            throw "NeuronImageDenoise: Failed to create processing action for the command.";
        else
        {
            qDebug() << "\nFunctions:\n";

            qDebug() << "denoise: remove background and flare effect.";

            qDebug() << "blur: remove steep background noise overlooked by denoising.";

            qDebug() << "max_ds: downsample to merge the gap between signals induced by previous steps. "
                        "Optional for reconstruction algorithms that allow small gaps.";

            qDebug() << "refine_soma: find soma based on a starting position by meanshift.";

            qDebug() << "integrated: execute the above procedures sequentially, i.e. denoise -> blur -> "
                        "max_ds -> refine_soma";

            qDebug() << "guo_enh: Guo's image enhancement.";

            qDebug() << "\nUsage:\n";

            qDebug() << "v3d -x neuron_image_denoise -f denoise -i in.tif -o out.tif -p "
                        "ada_interval 3,3,2 ada_sampling 3 flare_interval 8,8,2 flare_sampling 8 "
                        "flare_weight 0.02 atten_depth 4 flare_x true flare_y true";

            qDebug() << "v3d -x neuron_image_denoise -f blur -i in.tif -o out.tif -p blur_sigma 1,1,1";

            qDebug() << "v3d -x neuron_image_denoise -f max_ds -i in.tif -o out.tif -p scale 4,4,1";

            qDebug() << "v3d -x neuron_image_denoise -f refine_soma -i in.tif -o out.marker -p "
                        "start_pos 0.5,0.5,0.5 win_radius 20,20,20 normalized_start true ms_sigma 3,3,3 "
                        "gsdt true dt_threshold 0 test_count 50 z_thickness 1 connection_type 3";

            qDebug() << "v3d -x neuron_image_denoise -f integrated -i in.tif -o out.tif out.marker -p "
                        "[parameters related]";

            qDebug() << "v3d -x neuron_image_denoise -f guo_enh -i in.tif -o out.tif -p "
                        "enh_sigma 1,1,0.25 color_sigma 35 gain 5 cutoff 25 bilateral true fft true";

            qDebug() << "";
            if (func_name != tr("help"))
                throw "NeuronImageDenoise: Unknown command.";
        }
    }
    catch(const char* msg)
    {
        qDebug() << "";
        qDebug() << msg;
        qDebug() << "";
        flag = false;
    }
    catch(const exception& e)
    {
        qDebug() << "";
        qDebug() << e.what();
        qDebug() << "";
        flag = false;
    }

    catch(...)
    {
        qDebug() << "";
        qDebug() << "NeuronImageDenoise: Some error ocurred during execution.";
        qDebug() << "";
        flag = false;
    }
    return flag;
}

