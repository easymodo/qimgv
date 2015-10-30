#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QLinearGradient>
#include <QPropertyAnimation>
#include "../sourceContainers/thumbnail.h"
#include "../settings.h"

enum loadState { EMPTY, LOADING, LOADED };

class ThumbnailLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(qreal currentOpacity READ opacity WRITE setOpacity)

public:
    ThumbnailLabel(QWidget *parent = 0);
    ~ThumbnailLabel();

  //  void setGeometry(const QRect &rect);// Q_DECL_OVERRIDE;
  //  QRectF boundingRect() const;// Q_DECL_OVERRIDE;

    bool isLoaded();
    loadState state;
    void setThumbnail(Thumbnail *_thumbnail);

    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setHighlighted(bool x);
    bool isHighlighted();
    void setOpacity(qreal amount);
    qreal opacity();
    void readSettings();
    void applySettings();
    void setOpacityAnimated(qreal amount, int speed);

private:
    Qt::Orientation orientation;
    const int SHADOW_HEIGHT = 10;
    bool hovered, loaded;
    bool showLabel, showName;

    Thumbnail *thumbnail;
    qreal currentOpacity;
    bool highlighted;
    int borderW, borderH, thumbnailSize;
    QString infoString;
    QLinearGradient *shadowGradient;
    QRectF highlightRect, shadowRect, labelRect, nameRect;
    QColor *highlightColor, *outlineColor, *highlightColorBorder, *nameColor, *labelColor;
    QFont font;
    QFontMetrics *fm;

    void updateLabelWidth();
protected:
    virtual void paintEvent(QPaintEvent* event);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;// Q_DECL_OVERRIDE;

 //void enterEvent(QEvent *event);
 //void leaveEvent(QEvent *event);
};

#endif // THUMBNAILLABEL_H

