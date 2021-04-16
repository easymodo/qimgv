#include "bookmarksitem.h"

BookmarksItem::BookmarksItem(QString _dirName, QString _dirPath, QWidget *parent)
    : QWidget(parent), dirName(_dirName), dirPath(_dirPath), mHighlighted(false)
{
    this->setContentsMargins(0,0,0,0);
    layout.setContentsMargins(10,6,10,6);
    setAcceptDrops(true);
    dirNameLabel.setText(dirName);

    spacer = new QSpacerItem(16, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    folderIconWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    folderIconWidget.setIconPath(":/res/icons/common/menuitem/folder16.png");
    folderIconWidget.setMinimumSize(16, 16);
    folderIconWidget.installEventFilter(this);

    removeItemButton.setIconPath(":/res/icons/common/buttons/panel-small/remove12.png");
    removeItemButton.setMinimumSize(16, 16);
    removeItemButton.installEventFilter(this);

    removeItemButton.setAccessibleName("BookmarksItemRemoveLabel");

    connect(&removeItemButton, &IconButton::clicked, this, &BookmarksItem::onRemoveClicked);

    layout.addWidget(&folderIconWidget);
    layout.addWidget(&dirNameLabel);
    layout.addSpacerItem(spacer);
    layout.addWidget(&removeItemButton);

    setMouseTracking(true);

    setLayout(&layout);
}

QString BookmarksItem::path() {
    return dirPath;
}

void BookmarksItem::setHighlighted(bool mode) {
    if(mode != mHighlighted) {
        mHighlighted = mode;
        setProperty("highlighted", mHighlighted);
        style()->unpolish(this);
        style()->polish(this);
    }
}

void BookmarksItem::mouseReleaseEvent(QMouseEvent *event) {
    event->accept();
    if(event->button() == Qt::LeftButton)
        emit clicked(dirPath);
}

void BookmarksItem::mousePressEvent(QMouseEvent *event) {
    event->accept();
}

void BookmarksItem::onRemoveClicked() {
    emit removeClicked(dirPath);
}

void BookmarksItem::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BookmarksItem::dropEvent(QDropEvent *event) {
    QList<QString> paths;
    // TODO: QUrl gave me some issues previosly, test
    for(auto url : event->mimeData()->urls())
        paths << url.toLocalFile();
    emit droppedIn(paths, dirPath);
}

void BookmarksItem::dragEnterEvent(QDragEnterEvent *event) {
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
    setProperty("hover", true);
    update();
}

void BookmarksItem::dragLeaveEvent(QDragLeaveEvent *event) {
    Q_UNUSED(event)
    setProperty("hover", false);
    update();
}
