#include "imageinfooverlay.h"
#include "ui_imageinfooverlay.h"

ImageInfoOverlay::ImageInfoOverlay(FloatingWidgetContainer *parent) :
    OverlayWidget(parent),
    ui(new Ui::ImageInfoOverlay)
{
    ui->setupUi(this);
    ui->closeButton->setIconPath(":res/icons/common/overlay/close-dim16.png");
    ui->headerIcon->setIconPath(":res/icons/common/overlay/info16.png");
    entryStub.setFixedSize(280, 48);
    entryStub.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    connect(ui->closeButton,  &IconButton::clicked, this, &ImageInfoOverlay::hide);
    this->setPosition(FloatingWidgetPosition::RIGHT);

    if(parent)
        setContainerSize(parent->size());
}

ImageInfoOverlay::~ImageInfoOverlay() {
    delete ui;
    for(auto i = entries.count() - 1; i >= 0; i--)
        delete entries.takeAt(i);
}

void ImageInfoOverlay::setExifInfo(QVector<QPair<QString, QString>> info) {
    // remove/add entries
    int entryCount = entries.count();
    if(entryCount > info.count()) {
        for(auto i = entryCount - 1; i >= info.count(); i--) {
            ui->entryLayout->removeWidget(entries.last());
            delete entries.takeLast();
        }
    } else if(entryCount < info.count()) {
        for(auto i = entryCount; i < info.count(); i++) {
            entries.append(new EntryInfoItem(this));
            ui->entryLayout->addWidget(entries.last());
        }
    }
    for(int entryIdx = 0; entryIdx < info.count(); ++entryIdx)
        entries.at(entryIdx)->setInfo(info.at(entryIdx).first, info.at(entryIdx).second);

    // size the name column to fit the widest rendered label instead of
    // relying on a fixed pixel guess
    int nameColumnWidth = 0;
    for(auto *entry : entries)
        nameColumnWidth = qMax(nameColumnWidth, entry->nameWidthHint());
    nameColumnWidth += 4; // rounding safety margin
    for(auto *entry : entries)
        entry->setNameColumnWidth(nameColumnWidth);

    // Hiding/showing entryStub causes flicker,
    // so we just remove it from layout and clear the text.
    // It's still there but basically not visible
    if(entries.count()) {
        ui->entryLayout->removeWidget(&entryStub);
        entryStub.setText("");
    } else {
        ui->entryLayout->addWidget(&entryStub);
        entryStub.setText("<no metadata found>");
    }

    if(!isHidden()) {
        // wait for layout change (entry count, name column width or
        // wrapped value height may all have changed the overlay's size)
        qApp->processEvents();
        // reposition
        recalculateGeometry();
    }
}

void ImageInfoOverlay::show() {
    OverlayWidget::show();
    adjustSize();
    recalculateGeometry();
}

void ImageInfoOverlay::wheelEvent(QWheelEvent *event) {
    event->accept();
}
