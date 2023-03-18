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
            <<("integrated filter")
            <<("1. simple denoise")
            <<("2. gaussian blur")
            <<("3. max downsampling")
            << tr("about");
}


QStringList NeuronImageDenoise::funclist() const
{
    return QStringList()
            << tr("integrated_filter")
            << tr("denoise")
            << tr("blur")
            << tr("max_ds")
            << tr("guo_enh")
            << tr("search_soma")
            << tr("help");
}


void NeuronImageDenoise::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    try
    {
        QSharedPointer<BaseAction> action;

        if (menu_name == tr("integrated filter"))
            action = QSharedPointer<BaseAction>(new IntegratedFilterAction);
        else if (menu_name == tr("1. simple denoise"))
            action = QSharedPointer<BaseAction>(new DenoiseAction);
        else if (menu_name == tr("2. gaussian blur"))
            action = QSharedPointer<BaseAction>(new BlurAction);
        else if (menu_name == tr("3. max downsampling"))
            action = QSharedPointer<BaseAction>(new DownsampleAction);

        if (!action.isNull())
        {
            action->init(callback, parent);
            action->dialog();
        }
        else
            QMessageBox::about(0, "Neuron Image Denoise", "TODO");
    }
    catch(const char* msg)
    {
        QMessageBox::critical(0, "Error", msg);
    }
    catch(...)
    {
        QMessageBox::critical(0, "Error", "Unknown error ocurred during execution.");
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

        if (func_name == tr("integrated_filter"))
            action = QSharedPointer<BaseAction>(new IntegratedFilterAction);
        else if (func_name == tr("denoise"))
            action = QSharedPointer<BaseAction>(new DenoiseAction);
        else if (func_name == tr("blur"))
            action = QSharedPointer<BaseAction>(new BlurAction);
        else if (func_name == tr("max_ds"))
            action = QSharedPointer<BaseAction>(new DownsampleAction);
        else if (func_name == tr("guo_enh"))
            action = QSharedPointer<BaseAction>(new GuoEnhAction);
        else if (func_name == tr("search_soma"))
            action = QSharedPointer<BaseAction>(new SomaSearchAction);
        else
            command = false;

        if (!action.isNull())
        {
            action->init(input, output, callback, parent);
            action->parse();
            action->load();
            action->exec();
            action->save();
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

            qDebug() << "integrated_filter: execute the above procedures sequentially, "
                        "i.e. denoise -> blur -> max_ds";

            qDebug() << "guo_enh: Guo's image enhancement.";

            qDebug() << "search_soma: Search soma by finding maxima in GSDT.";

            qDebug() << "\nUsage:\n";

            qDebug() << "v3d -x neuron_image_denoise -f denoise -i in.tif -o out.tif -p "
                        "ada_interval 3,3,2 ada_sampling 3 flare_interval 8,8,2 flare_sampling 8 "
                        "flare_weight 0.02 atten_depth 4 flare_x true flare_y true";

            qDebug() << "v3d -x neuron_image_denoise -f blur -i in.tif -o out.tif -p blur_sigma 1,1,1";

            qDebug() << "v3d -x neuron_image_denoise -f max_ds -i in.tif -o out.tif -p scale 4,4,1";

            qDebug() << "v3d -x neuron_image_denoise -f integrated_filter -i in.tif -o out.tif out.marker -p "
                        "[parameters related]";

            qDebug() << "v3d -x neuron_image_denoise -f guo_enh -i in.tif -o out.tif -p "
                        "enh_sigma 1,1,0.25 color_sigma 35 gain 5 cutoff 25 bilateral true fft true";

            qDebug() << "v3d -x neuron_image_denoise -f search_soma -i in.tif in.marker -o out.marker -p "
                        "win_radius 15,15,15 bg_thr 0 z_thickness 1 cnn_type 3 test_count 10";

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

