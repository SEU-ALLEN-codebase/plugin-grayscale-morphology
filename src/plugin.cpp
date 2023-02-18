/* plugin.cpp
 * TODO
 * 2022-12-20 : by Zuohan
 */
 
#include "plugin.h"
#include "plugin_actions.h"
#include <QMessageBox>

using namespace std;


QStringList NeuronImageRectifier::menulist() const
{
    return QStringList()
            << tr("integrated")
            << tr("high_pass_filter")
            << tr("downsampling")
            << tr("enhancement")
            << tr("sparse_auto_thr")
            << tr("thr_hist_eq")
            << tr("meanshift_soma_refinement")
            << tr("about");
}


QStringList NeuronImageRectifier::funclist() const
{
    return QStringList()
            << tr("integrated")
            << tr("high_pass_filter")
            << tr("ada_thr")
            << tr("morpho_thr")
            << tr("sort_filter")
            << tr("downsampling")
            << tr("enhancement")
            << tr("sparse_auto_thr")
            << tr("thr_hist_eq")
            << tr("meanshift_soma_refinement")
            << tr("help");
}


void NeuronImageRectifier::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    try
    {
        QSharedPointer<BaseAction> action;

        if (menu_name == tr("integrated"))
            action = QSharedPointer<BaseAction>(new IntegratedAction);
        else if (menu_name == tr("ada_thr"))
            action = QSharedPointer<BaseAction>(new AdaThresholdAction);
        else if (menu_name == tr("morpho_thr"))
            action = QSharedPointer<BaseAction>(new MorphoThresholdAction);
        else if (menu_name == tr("sort_filter"))
            action = QSharedPointer<BaseAction>(new SortFilterAction);
        else if (menu_name == tr("high_pass_filter"))
            action = QSharedPointer<BaseAction>(new GaussianHighPassFilterAction);
        else if (menu_name == tr("downsampling"))
            action = QSharedPointer<BaseAction>(new DownsamplingAction);
        else if (menu_name == tr("enhancement"))
            action = QSharedPointer<BaseAction>(new EnhancementAction);
        else if (menu_name == tr("sparse_auto_thr"))
            action = QSharedPointer<BaseAction>(new SparseAutoThresholdAction);
        else if (menu_name == tr("thr_hist_eq"))
            action = QSharedPointer<BaseAction>(new ThresholdedHistogramEqualizationAction);
        else if (menu_name == tr("meanshift_soma_refinement"))
            action = QSharedPointer<BaseAction>(new MeanshiftSomaRefinementAction);

        if (!action.isNull())
        {
            action->init(callback, parent);
            action->dialog();
            action->exec();
        }
        else
            QMessageBox::about(0, "Neuron Image Signal Processing", "TODO");
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


bool NeuronImageRectifier::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
    bool flag = true;
    try
    {
        QSharedPointer<BaseAction> action;
        bool command = true;

        if (func_name == tr("integrated"))
            action = QSharedPointer<BaseAction>(new IntegratedAction);
        else if (func_name == tr("ada_thr"))
            action = QSharedPointer<BaseAction>(new AdaThresholdAction);
        else if (func_name == tr("morpho_thr"))
            action = QSharedPointer<BaseAction>(new MorphoThresholdAction);
        else if (func_name == tr("sort_filter"))
            action = QSharedPointer<BaseAction>(new SortFilterAction);
        else if (func_name == tr("high_pass_filter"))
            action = QSharedPointer<BaseAction>(new GaussianHighPassFilterAction);
        else if (func_name == tr("downsampling"))
            action = QSharedPointer<BaseAction>(new DownsamplingAction);
        else if (func_name == tr("enhancement"))
            action = QSharedPointer<BaseAction>(new EnhancementAction);
        else if (func_name == tr("sparse_auto_thr"))
            action = QSharedPointer<BaseAction>(new SparseAutoThresholdAction);
        else if (func_name == tr("thr_hist_eq"))
            action = QSharedPointer<BaseAction>(new ThresholdedHistogramEqualizationAction);
        else if (func_name == tr("meanshift_soma_refinement"))
            action = QSharedPointer<BaseAction>(new MeanshiftSomaRefinementAction);
        else
            command = false;

        if (!action.isNull())
        {
            action->init(input, output, callback, parent);
            action->parse();
            action->exec();
        }
        else if (command)
            throw "Plugin: failed to create processing action for the command.";
        else
        {
            qDebug() << "";
            qDebug() << "Functions:";
            qDebug() << "";
            qDebug() << "high_pass_filter: make neuron fibers standing out and remove flare using gaussian filter";
            qDebug() << "ada_thr: make neuron fibers standing out and remove flare, milder than gaussian filter";
            qDebug() << "sort_filter: sort filter to remove speckle noise and enhance thin fibers";
            qDebug() << "morpho_thr: use a structure element to filter an image";
            qDebug() << "downsampling: down sample based on max";
            qDebug() << "enhancement: Guo's image enhancement";
            qDebug() << "sparse_auto_thr: automatic thresholding using percentile and triangle thresholding";
            qDebug() << "thr_hist_eq: given a lower threshold, apply histogram equalization";
            qDebug() << "meanshift_soma_refinement: find soma based on a starting position by meanshift";
            qDebug() << "integrated: execute the above procedures sequentially, i.e. ada_thr -> auto_thr -> "
                        "hist_eq -> morpho_thr -> meanshift";
            qDebug() << "";
            qDebug() << "Usage:";
            qDebug() << "";
            qDebug() << "vaa3d -x neuron_image_rectify -f high_pass_filter -i in.tif -o out.tif -p hp_sigma 8,8,2 hp_skew 0,0,-1";
            qDebug() << "vaa3d -x neuron_image_rectify -f ada_thr -i in.tif -o out.tif -p ada_interval 4,4,1 ada_sampling 5";
            qDebug() << "vaa3d -x neuron_image_rectify -f morpho_thr -i in.tif -o out.tif -p morpho_win 2,2,1 morpho_thr 0";
            qDebug() << "vaa3d -x neuron_image_rectify -f sort_filter -i in.tif -o out.tif -p sort_radius 1,1,0 sort_order 3";
            qDebug() << "vaa3d -x neuron_image_rectify -f downsampling -i in.tif -o out.tif -p scale 4,4,1";
            qDebug() << "vaa3d -x neuron_image_rectify -f enhancement -i in.tif -o out.tif -p enh_sigma 1,1,0.3 "
                        "color_sigma 35 gain 5 cutoff 25 bilateral true fft true";
            qDebug() << "vaa3d -x neuron_image_rectify -f sparse_auto_thr -i in.tif -o out.marker -p fg_percentile 0.01 "
                        "n_bin_float 256";
            qDebug() << "vaa3d -x neuron_image_rectify -f thr_hist_eq -i in.tif -o out.tif out.marker -p hist_eq_thr 0 "
                        "dtype 1 n_bin_float 256";
            qDebug() << "vaa3d -x neuron_image_rectify -f meanshift_soma_refinement -i in.tif -o out.marker -p "
                        "start_pos 0.5,0.5,0.5 win_radius 20,20,20 normalized_start true ms_sigma 3,3,3 gsdt true dt_threshold 0 "
                        "test_count 50 z_thickness 1 connection_type 3";
            qDebug() << "vaa3d -x neuron_image_rectify -f integrated -i in.tif -o out.tif out.marker -p [any parameter above]";
            qDebug() << "";
            if (func_name != tr("help"))
                throw "Plugin: Unknown command.";
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
        qDebug() << "Plugin: Some error ocurred during execution.";
        qDebug() << "";
        flag = false;
    }
    return flag;
}

