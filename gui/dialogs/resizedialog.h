#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QDesktopWidget>
#include <QDebug>

namespace Ui {
    class ResizeDialog;
}

class ResizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResizeDialog(QSize initialSize, QWidget *parent = nullptr);
    ~ResizeDialog();
    QSize newSize();

public slots:
    int exec();
private:
    Ui::ResizeDialog *ui;
    QSize originalSize, targetSize, desktopSize;
    void updateToTargetValues();
    int lastEdited; // 0 - width, 1 - height
    void resetResCheckBox();

private slots:
    void widthChanged(int);
    void heightChanged(int);
    void sizeSelect();

    void setCommonResolution(int);
    void reset();
    void fitDesktop();
    void fillDesktop();
    void onAspectRatioCheckbox();
signals:
    void sizeSelected(QSize);
};

#endif // RESIZEDIALOG_H
