#ifndef STYLEDCOMBOBOX_H
#define STYLEDCOMBOBOX_H

#include <QComboBox>
#include <QPainter>
#include <QKeyEvent>
#include "settings.h"
#include "utils/imagelib.h"

class StyledComboBox : public QComboBox
{
public:
    StyledComboBox(QWidget *parent = nullptr);
    void setIconPath(QString path);

protected:
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    bool hiResPixmap;
    QPixmap downArrow;
    qreal dpr, pixmapDrawScale;
};

#endif // STYLEDCOMBOBOX_H
