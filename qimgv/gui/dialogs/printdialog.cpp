#include "printdialog.h"
#include "ui_printdialog.h"

PrintDialog::PrintDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PrintDialog), targetPrinter(&pdfPrinter)
{
    ui->setupUi(this);

    ui->previewLabel->setContentsMargins(0,0,0,0);

    pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
    pdfPrinter.setPageSize(QPageSize(QPageSize::A4));
    pdfPrinter.setOutputFileName(ui->pdfPathEdit->text());

    setOutputPath(ui->pdfPathEdit->text());

    QStringList printerList = QPrinterInfo::availablePrinterNames();
    if(printerList.isEmpty()) {
        ui->printerListComboBox->hide();
        ui->pdfCheckBox->setChecked(true);
        onPdfChecked(true);
        ui->pdfCheckBox->setEnabled(false);
    } else {
        ui->printerListPlaceholder->hide();
        ui->printerListComboBox->addItems(printerList);
        ui->printerListComboBox->setCurrentText(QPrinterInfo::defaultPrinterName());
        onPrinterSelected(QPrinterInfo::defaultPrinterName());
        ui->pdfCheckBox->setChecked(false);
        onPdfChecked(false);
    }
    connect(ui->cancelButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->printButton, &QPushButton::clicked, this, &PrintDialog::print);
    connect(ui->pdfCheckBox, &QCheckBox::toggled, this, &PrintDialog::onPdfChecked);
    connect(ui->pdfPathEdit, &QLineEdit::textChanged, this, &PrintDialog::onOutputPathEdited);
    connect(ui->printerListComboBox, &QComboBox::currentTextChanged, this, &PrintDialog::onPrinterSelected);
    connect(ui->portrait, &QRadioButton::toggled, this, &PrintDialog::updatePrintLayout);
    connect(ui->pdfPathSelectButton, &QPushButton::clicked, this, &PrintDialog::pdfPathDialog);
}

PrintDialog::~PrintDialog() {
    if(printer)
        delete printer;
    delete ui;
}

void PrintDialog::setImage(std::shared_ptr<const QImage> _img) {
    img = _img;
    updatePrintLayout();
}

void PrintDialog::onPdfChecked(bool mode) {
    ui->pdfPathWidget->setDisabled(!mode);
    ui->printerListWidget->setDisabled(mode);
    if(ui->pdfCheckBox->isChecked())
        targetPrinter = &pdfPrinter;
    else
        targetPrinter = printer;
    updatePrintLayout();
}

void PrintDialog::setOutputPath(QString path) {
    ui->pdfPathEdit->blockSignals(true);
    ui->pdfPathEdit->setText(path);
    ui->pdfPathEdit->blockSignals(false);
    onOutputPathEdited(path);
}

void PrintDialog::onOutputPathEdited(QString path) {
    if(path.isEmpty()) // prevent fallback to a real printer
        path = " ";
    pdfPrinter.setOutputFileName(path);
}

void PrintDialog::pdfPathDialog() {
    QString pdfPath = QFileDialog::getSaveFileName(this, tr("Choose pdf location"),
                                            ui->pdfPathEdit->text(), "*.pdf");
    if(!pdfPath.isEmpty())
        setOutputPath(pdfPath);
}

void PrintDialog::updatePrintLayout() {
    if(!img || !targetPrinter)
        return;
    if(ui->portrait->isChecked())
        targetPrinter->setPageOrientation(QPageLayout::Portrait);
    else
        targetPrinter->setPageOrientation(QPageLayout::Landscape);
    pageRect = QRectF(QPoint(0,0), targetPrinter->pageRect(QPrinter::DevicePixel).size());
    imgRect = img->rect();
    QRectF fullRect = targetPrinter->pageLayout().fullRect();

    // todo: fit buttons
    bool scaleToFit = true;
    // downscale / upscale
    if(scaleToFit || imgRect.width() > pageRect.width() || imgRect.height() > imgRect.height())
        imgRect.setSize(imgRect.size().scaled(pageRect.size(), Qt::KeepAspectRatio));
    // -- fit topleft (default)
    //imgRect.moveTopLeft(pageRect.topLeft());
    // -- fit top center
    imgRect.moveCenter(pageRect.center());
    imgRect.moveTop(pageRect.top());
    // -- fit center
    //imgRect.moveCenter(pageRect.center());

    // ------------ thumbnail
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
    QPixmap imgPixmap = QPixmap::fromImage(img->scaled(imgRectScaled.size() * qApp->devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    imgPixmap.setDevicePixelRatio(qApp->devicePixelRatio());
    QPainter p(&pagePixmap);
    p.fillRect(pagePixmap.rect(), QColor(245,245,245));
    p.drawPixmap(imgRectScaled.left(), imgRectScaled.top(), imgPixmap);
    ui->previewLabel->setPixmap(pagePixmap);
}

void PrintDialog::onPrinterSelected(QString name) {
    if(printer)
        delete printer;
    printer = new QPrinter(QPrinterInfo::printerInfo(name));
    updatePrintLayout();
}

void PrintDialog::print() {
    if(!img || !targetPrinter) {
        close();
        return;
    }
    QPainter p(targetPrinter);
    p.drawImage(imgRect, *img);
    close();
}
