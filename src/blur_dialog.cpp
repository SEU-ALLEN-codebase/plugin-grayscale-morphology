#include "blur_dialog.h"
#include "ui_blur_dialog.h"

BlurDialog::BlurDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlurDialog)
{
    ui->setupUi(this);
}

BlurDialog::~BlurDialog()
{
    delete ui;
}

void BlurDialog::setParams(GaussianBlurring& handler)
{
    handler.sigma = QVector3D(ui->sbx->value(), ui->sby->value(), ui->sbz->value());
}


bool BlurDialog::overwrite()
{
    return ui->over->isChecked();
}
