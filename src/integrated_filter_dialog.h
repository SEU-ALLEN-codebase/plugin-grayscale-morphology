#ifndef INTEGRATED_FILTER_DIALOG_H
#define INTEGRATED_FILTER_DIALOG_H

#include <QDialog>
#include "handlers.h"

namespace Ui {
class IntegratedFilterDialog;
}

class IntegratedFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IntegratedFilterDialog(QWidget *parent = nullptr);
    ~IntegratedFilterDialog();
    void setParams(DenoisingThreshold& h1, GaussianBlurring& h2, Downsampling& h3);
    bool overwrite();

private:
    Ui::IntegratedFilterDialog *ui;
};

#endif // INTEGRATED_FILTER_DIALOG_H
