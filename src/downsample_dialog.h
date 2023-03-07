#ifndef DOWNSAMPLE_DIALOG_H
#define DOWNSAMPLE_DIALOG_H

#include <QDialog>
#include "handlers.h"

namespace Ui {
class DownsampleDialog;
}

class DownsampleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownsampleDialog(QWidget *parent = nullptr);
    ~DownsampleDialog();
    void setParams(Downsampling& handler);
    bool overwrite();

private:
    Ui::DownsampleDialog *ui;
};

#endif // DOWNSAMPLE_DIALOG_H
