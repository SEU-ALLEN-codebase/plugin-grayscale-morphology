#include "downsample_dialog.h"
#include "ui_downsample_dialog.h"

DownsampleDialog::DownsampleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownsampleDialog)
{
    ui->setupUi(this);
}

DownsampleDialog::~DownsampleDialog()
{
    delete ui;
}

void DownsampleDialog::setParams(Downsampling& handler)
{
    handler.scale = QVector3D(ui->sbx->value(), ui->sby->value(), ui->sbz->value());
}


bool DownsampleDialog::overwrite()
{
    return ui->over->isChecked();
}
