#ifndef INFOBAR_H
#define INFOBAR_H

#include <QWidget>

namespace Ui {
class InfoBar;
}

class InfoBar : public QWidget
{
    Q_OBJECT

public:
    explicit InfoBar(QWidget *parent = nullptr);
    ~InfoBar();

public slots:
    void setInfo(int fileIndex, int fileCount, QString fileName, QSize imageSize, int fileSize);
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::InfoBar *ui;
};

#endif // INFOBAR_H
