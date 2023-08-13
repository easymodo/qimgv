#include "printdialog.h"
#include "ui_printdialog.h"

PrintDialog::PrintDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PrintDialog)
{
    ui->setupUi(this);
    ui->previewLabel->setContentsMargins(0,0,0,0);
    pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
    pdfPrinter.setPageSize(QPageSize(QPageSize::A4));
    pdfPrinter.setOutputFileName(" ");
    QStringList printerList = QPrinterInfo::availablePrinterNames();
    if(printerList.isEmpty()) {
        ui->printerListComboBox->hide();
        ui->printButton->setEnabled(false);
        ui->exportPdfButton->setFocus();
    } else {
        ui->printerListPlaceholder->hide();
        ui->printerListComboBox->addItems(printerList);
        ui->printerListComboBox->setCurrentText(QPrinterInfo::defaultPrinterName());
        if(printerList.contains(settings->lastPrinter()))
            onPrinterSelected(settings->lastPrinter());
        else
            onPrinterSelected(QPrinterInfo::defaultPrinterName());
        printPdfDefault = settings->printPdfDefault();
    }
    ui->color->setChecked(settings->printColor());
    setLandscape(settings->printLandscape());
    ui->fitToPageCheckBox->setChecked(settings->printFitToPage());
    if(printPdfDefault)
        ui->exportPdfButton->setFocus();
    // ui signals
    connect(ui->cancelButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->printButton, &QPushButton::clicked, this, &PrintDialog::print);
    connect(ui->exportPdfButton, &QPushButton::clicked, this, &PrintDialog::exportPdf);
    connect(ui->printerListComboBox, &QComboBox::currentTextChanged, this, &PrintDialog::onPrinterSelected);
    connect(ui->landscape, &QRadioButton::toggled, this, &PrintDialog::setLandscape);
    connect(ui->fitToPageCheckBox, &QCheckBox::toggled, this, &PrintDialog::updatePreview);
    connect(ui->color, &QRadioButton::toggled, this, &PrintDialog::updatePreview);
}

void PrintDialog::saveSettings() {
    settings->setPrintLandscape(ui->landscape->isChecked());
    settings->setPrintColor(ui->color->isChecked());
    settings->setPrintFitToPage(ui->fitToPageCheckBox->isChecked());
    settings->setPrintPdfDefault(printPdfDefault);
    if(!ui->printerListComboBox->currentText().isEmpty())
        settings->setLastPrinter(ui->printerListComboBox->currentText());
}

PrintDialog::~PrintDialog() {
    saveSettings();
    if(printer)
        delete printer;
    delete ui;
}

void PrintDialog::setImage(std::shared_ptr<const QImage> _img) {
    img = _img;
    updatePreview();
}

void PrintDialog::setOutputPath(QString path) {
    if(path.isEmpty())
        path = " ";
    pdfPrinter.setOutputFileName(path);
}

QString PrintDialog::pdfPathDialog() {
    return QFileDialog::getSaveFileName(this, tr("Choose pdf location"), pdfPrinter.outputFileName(), "*.pdf");
}

void PrintDialog::updatePreview() {
    if(!img)
        return;
    QPrinter *targetPrinter = printer;
    if(!targetPrinter)
        targetPrinter = &pdfPrinter;
    auto imgRect = getImagePrintRect(targetPrinter);
    QRectF fullRect = targetPrinter->pageLayout().fullRectPixels(targetPrinter->resolution());
    // margins
    QMarginsF margins(targetPrinter->pageLayout().marginsPixels(targetPrinter->resolution()));
    // scaled page with margins
    QRect fullRectScaled( QRectF(QPointF(0,0), fullRect.size().scaled(ui->previewLabel->size(), Qt::KeepAspectRatio)).toRect() );
    qreal scale = fullRectScaled.width() / fullRect.width();
    // scaled image rect with margins (not accurate, but good enough for a preview)
    QRect imgRectScaled(QRectF((imgRect.left() + margins.left()) * scale, (imgRect.top() + margins.top()) * scale,
                               imgRect.width() * scale, imgRect.height() * scale).toRect());
    QPixmap pagePixmap(fullRectScaled.size() * qApp->devicePixelRatio());
    pagePixmap.setDevicePixelRatio(qApp->devicePixelRatio());
    auto scaledImg = img->scaled(imgRectScaled.size() * qApp->devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if(ui->grayscale->isChecked())
        scaledImg = scaledImg.convertToFormat(QImage::Format_Grayscale8);
    scaledImg.setDevicePixelRatio(qApp->devicePixelRatio());
    QPainter p(&pagePixmap);
    p.fillRect(pagePixmap.rect(), QColor(255,255,255));
    p.drawImage(imgRectScaled.left(), imgRectScaled.top(), scaledImg);
    // page border for white window bg
    QPalette palette;
    QColor sys_window = palette.window().color();
    if(sys_window.valueF() > 0.45f) {
        p.setOpacity(0.25f);
        p.setPen(Qt::black);
        p.drawRect(QRectF(QPointF(0.5f, 0.5f), QSizeF(pagePixmap.size() / qApp->devicePixelRatio() - QSizeF(1.0f, 1.0f))));
    }
    ui->previewLabel->setPixmap(pagePixmap);
}

QRectF PrintDialog::getImagePrintRect(QPrinter *pr) {
    QRectF imgRect;
    if(!pr || !img)
        return QRect();
    QRectF pageRect = QRectF(QPoint(0,0), pr->pageRect(QPrinter::DevicePixel).size());
    imgRect = img->rect();
    // downscale / upscale
    if(ui->fitToPageCheckBox->isChecked() || imgRect.width() > pageRect.width() || imgRect.height() > pageRect.height())
        imgRect.setSize(imgRect.size().scaled(pageRect.size(), Qt::KeepAspectRatio));
    // align top center
    imgRect.moveCenter(pageRect.center());
    imgRect.moveTop(pageRect.top());
    return imgRect;
}

void PrintDialog::setLandscape(bool mode) {
    ui->landscape->blockSignals(true);
    ui->landscape->setChecked(mode);
    ui->landscape->blockSignals(false);
    QPageLayout::Orientation orientation = QPageLayout::Portrait;
    if(mode)
        orientation = QPageLayout::Landscape;
    if(printer)
        printer->setPageOrientation(orientation);
    pdfPrinter.setPageOrientation(orientation);
    updatePreview();
}

void PrintDialog::onPrinterSelected(QString name) {
    if(printer)
        delete printer;
    printer = new QPrinter(QPrinterInfo::printerInfo(name));
    updatePreview();
}

void PrintDialog::print() {
    if(!img || !printer) {
        close();
        return;
    }
    if(ui->color->isChecked())
        printer->setColorMode(QPrinter::Color);
    else
        printer->setColorMode(QPrinter::GrayScale);
    QPainter p(printer);
    p.drawImage(getImagePrintRect(printer), *img);
    printPdfDefault = false;
    close();
}

void PrintDialog::exportPdf() {
    if(!img || pdfPrinter.outputFileName().isEmpty()) {
        close();
        return;
    }
    auto path = pdfPathDialog();
    if(path.isEmpty())
        return;
    pdfPrinter.setOutputFileName(path);
    if(ui->color->isChecked())
        pdfPrinter.setColorMode(QPrinter::Color);
    else
        pdfPrinter.setColorMode(QPrinter::GrayScale);
    QPainter p(&pdfPrinter);
    p.drawImage(getImagePrintRect(&pdfPrinter), *img);
    printPdfDefault = true;
    close();
}
