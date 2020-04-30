#include "bookmarksitem.h"

BookmarksItem::BookmarksItem(QString _dirName, QString _dirPath, QWidget *parent)
    : QWidget(parent), dirName(_dirName), dirPath(_dirPath), mHighlighted(false)
{
    this->setContentsMargins(0,0,0,0);
    layout.setContentsMargins(4,6,4,6);
    setAcceptDrops(true);
    dirNameLabel.setText(dirName);

    spacer = new QSpacerItem(16, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    folderIconLabel.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    folderIconLabel.setPixmap(QIcon(":/res/icons/menuitem/folder16.png").pixmap(16, 16));
    folderIconLabel.installEventFilter(this);

    removeIconLabel.setIconPath(":/res/icons/buttons/remove12.png");
    removeIconLabel.setMinimumSize(16, 16);
    removeIconLabel.installEventFilter(this);

    removeIconLabel.setAccessibleName("BookmarksItemRemoveLabel");

    connect(&removeIconLabel, &IconButton::clicked, this, &BookmarksItem::onRemoveClicked);

    layout.addWidget(&folderIconLabel);
    layout.addWidget(&dirNameLabel);
    layout.addSpacerItem(spacer);
    layout.addWidget(&removeIconLabel);

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
    Q_UNUSED(event)
    if(event->button() == Qt::LeftButton)
        emit clicked(dirPath);
}

void BookmarksItem::onRemoveClicked() {
    emit removeClicked(dirPath);
}

void BookmarksItem::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool BookmarksItem::eventFilter(QObject *watched, QEvent *event) {
    if(event->type() == QEvent::Paint) {
        if(watched == &folderIconLabel) {
            QLabel *label = dynamic_cast<QLabel*>(watched);
            QPainter painter(label);
            label->style()->drawItemPixmap(&painter, label->rect(), Qt::AlignHCenter | Qt::AlignVCenter, *label->pixmap());
            return true;
        }
    }
    return false;
}

void BookmarksItem::dropEvent(QDropEvent *event) {
    emit droppedIn(event->mimeData()->urls(), dirPath);
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
