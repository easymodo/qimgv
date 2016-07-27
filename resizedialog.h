#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>

class ResizeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResizeDialog(QWidget *parent = 0);
    QSize newSize();

private:
    QVBoxLayout vbox;
    QHBoxLayout hbox, hbox2, hbox3;
    QPushButton okButton, cancelButton;
    QCheckBox aspect;
    QSpinBox width, height;
    QSize originalSize, targetSize;
    void updateToTargetValues();

private slots:
    void widthChanged(int);
    void heightChanged(int);
    void sizeSelect();

signals:
    void sizeSelected(QSize);
};

#endif // RESIZEDIALOG_H
