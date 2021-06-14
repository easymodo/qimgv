#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrinterInfo>
#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include "components/thumbnailer/thumbnailer.h"

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog {
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent = nullptr);
    ~PrintDialog();
    void setImage(std::shared_ptr<const QImage> _img);
    void setOutputPath(QString path);

private slots:
    void print();
    void onPdfChecked(bool mode);
    void updatePrintLayout();
    void onPrinterSelected(QString name);
    void onOutputPathEdited(QString path);
    void pdfPathDialog();

private:
    Ui::PrintDialog *ui;
    std::shared_ptr<const QImage> img = nullptr;
    QPrinter pdfPrinter;
    QPrinter *printer = nullptr, *targetPrinter = nullptr;
    QRectF pageRect, imgRect;
};

#endif // PRINTDIALOG_H
