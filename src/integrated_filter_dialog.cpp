#include "integrated_filter_dialog.h"
#include "ui_integrated_filter_dialog.h"

IntegratedFilterDialog::IntegratedFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntegratedFilterDialog)
{
    ui->setupUi(this);
}

IntegratedFilterDialog::~IntegratedFilterDialog()
{
    delete ui;
}


void IntegratedFilterDialog::setParams(DenoisingThreshold& h1, GaussianBlurring& h2, Downsampling& h3)
{
    h1.ada_interval = QVector3D(ui->sb_at_x->value(), ui->sb_at_y->value(), ui->sb_at_z->value());
    h1.ada_sampling = ui->sb_at_n->value();
    h1.flare_interval = QVector3D(ui->sb_fr_x->value(), ui->sb_fr_y->value(), ui->sb_fr_z->value());
    h1.flare_sampling = ui->sb_fr_n->value();
    h1.flare_weight = ui->sb_fr_w->value();
    h1.atten_depth = ui->sb_fr_d->value();
    h1.flare_x = ui->fr_x->isChecked();
    h1.flare_y = ui->fr_y->isChecked();
    h2.sigma = QVector3D(ui->sbx->value(), ui->sby->value(), ui->sbz->value());
    h3.scale = QVector3D(ui->sbx_2->value(), ui->sby_2->value(), ui->sbz_2->value());
}


bool IntegratedFilterDialog::overwrite()
{
    return ui->over->isChecked();
}
