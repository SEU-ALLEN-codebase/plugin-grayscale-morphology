#ifndef DENOISE_DIALOG_H
#define DENOISE_DIALOG_H

#include <QDialog>
#include "handlers.h"

namespace Ui {
class DenoiseDialog;
}

class DenoiseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DenoiseDialog(QWidget *parent = nullptr);
    ~DenoiseDialog();
    void setParams(DenoisingThreshold& handler);
    bool overwrite();

private:
    Ui::DenoiseDialog *ui;
};

#endif // DENOISE_DIALOG_H
