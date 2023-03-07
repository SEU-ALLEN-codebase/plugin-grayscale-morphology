#ifndef BLUR_DIALOG_H
#define BLUR_DIALOG_H

#include <QDialog>
#include "handlers.h"

namespace Ui {
class BlurDialog;
}

class BlurDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BlurDialog(QWidget *parent = nullptr);
    ~BlurDialog();
    void setParams(GaussianBlurring& handler);
    bool overwrite();

private:
    Ui::BlurDialog *ui;
};

#endif // BLUR_DIALOG_H
