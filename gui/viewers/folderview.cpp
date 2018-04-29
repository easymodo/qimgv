#include "folderview.h"

FolderView::FolderView(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Folder View"));
    setLayout(layout);
}

FolderView::~FolderView() {
    delete layout;
}
