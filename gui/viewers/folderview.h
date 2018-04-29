#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class FolderView : public QWidget
{
    Q_OBJECT
public:
    explicit FolderView(QWidget *parent = nullptr);
    ~FolderView();
signals:

public slots:

private:
    QVBoxLayout *layout;
};

#endif // FOLDERVIEW_H
