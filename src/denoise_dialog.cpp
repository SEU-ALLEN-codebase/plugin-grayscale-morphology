#include "denoise_dialog.h"
#include "ui_denoise_dialog.h"

DenoiseDialog::DenoiseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DenoiseDialog)
{
    ui->setupUi(this);
}

DenoiseDialog::~DenoiseDialog()
{
    delete ui;
}


void DenoiseDialog::setParams(DenoisingThreshold& handler)
{
    handler.ada_interval = QVector3D(ui->sb_at_x->value(), ui->sb_at_y->value(), ui->sb_at_z->value());
    handler.ada_sampling = ui->sb_at_n->value();
    handler.flare_interval = QVector3D(ui->sb_fr_x->value(), ui->sb_fr_y->value(), ui->sb_fr_z->value());
    handler.flare_sampling = ui->sb_fr_n->value();
    handler.flare_weight = ui->sb_fr_w->value();
    handler.atten_depth = ui->sb_fr_d->value();
    handler.flare_x = ui->fr_x->isChecked();
    handler.flare_y = ui->fr_y->isChecked();
}


bool DenoiseDialog::overwrite()
{
    return ui->over->isChecked();
}
