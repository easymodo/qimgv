#ifndef ENTRYINFOITEM_H
#define ENTRYINFOITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

class EntryInfoItem : public QWidget
{
    Q_OBJECT
public:
    explicit EntryInfoItem(QWidget *parent = nullptr);
    void setInfo(QString _name, QString _value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QString name;
    QString value;
    QHBoxLayout layout;
    QLabel nameLabel, valueLabel;
};

#endif // ENTRYINFOITEM_H
