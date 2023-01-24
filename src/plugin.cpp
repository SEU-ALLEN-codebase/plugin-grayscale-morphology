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
        else if (menu_name == tr("high_pass_filter"))
            action = QSharedPointer<BaseAction>(new HighPassFilterAction);
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
        else if (func_name == tr("high_pass_filter"))
            action = QSharedPointer<BaseAction>(new HighPassFilterAction);
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
        else if (!command)
            throw "Plugin: failed to create processing action for the command.";
        else
        {
            qDebug() << "TODO";
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

