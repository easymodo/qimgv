#include "resizedialog.h"

ResizeDialog::ResizeDialog(QWidget *parent) : QDialog(parent) {
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Resize image (mockup)");
    //setWindowIcon(QIcon(":/res/pepper32.png"));

    //set up layout
    this->setFixedSize(320, 150);

    aspect.setText("Keep aspect ratio");
    aspect.setChecked(true);
    hbox.addWidget(&aspect);

    width.setMinimumSize(100,30);
    height.setMinimumSize(100,30);
    hbox2.addWidget(&width);
    hbox2.addWidget(new QLabel(" x "));
    hbox2.addWidget(&height);
    hbox2.addStretch(0);

    okButton.setText("OK");
    cancelButton.setText("Cancel");
    hbox3.addStretch(0);
    hbox3.addWidget(&okButton);
    hbox3.addWidget(&cancelButton);

    vbox.addSpacing(10);
    vbox.addLayout(&hbox);
    vbox.addSpacing(5);
    vbox.addLayout(&hbox2);
    vbox.addSpacing(5);
    vbox.addLayout(&hbox3);
    this->setLayout(&vbox);
    width.setFocus();

    //test
    originalSize.setWidth(1980);
    originalSize.setHeight(1080);
    targetSize = originalSize;
    width.setRange(1, 65535); // should be enough.. right?
    height.setRange(1, 65535);
    width.setKeyboardTracking(false);
    height.setKeyboardTracking(false);
    width.setValue(originalSize.width());
    height.setValue(originalSize.height());


    connect(&width, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
    connect(&height, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));
    connect(&cancelButton, SIGNAL(pressed()), this, SLOT(reject()));
    connect(&okButton, SIGNAL(pressed()), this, SLOT(sizeSelect()));
}

void ResizeDialog::sizeSelect() {
    if(targetSize != originalSize)
        emit sizeSelected(targetSize);
    this->accept();
}

QSize ResizeDialog::newSize() {
    return targetSize;
}

void ResizeDialog::widthChanged(int newWidth) {
    if(width.hasFocus()) {
        float factor = (float)newWidth/originalSize.width();
        targetSize.setWidth(newWidth);
        if(aspect.isChecked()) {
            targetSize.setHeight(originalSize.height() * factor);
        }
        updateToTargetValues();
    }
}

void ResizeDialog::heightChanged(int newHeight) {
    if(height.hasFocus()) {
        float factor = (float)newHeight/originalSize.height();
        targetSize.setHeight(newHeight);
        if(aspect.isChecked()) {
            targetSize.setWidth(originalSize.width() * factor);
        }
        updateToTargetValues();
    }
}

void ResizeDialog::updateToTargetValues() {
    width.setValue(targetSize.width());
    height.setValue(targetSize.height());
}
