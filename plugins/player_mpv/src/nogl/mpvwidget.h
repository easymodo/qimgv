#pragma once

#include <QtCore/QMetaObject>

#include <mpv/client.h>
#include "qthelper.hpp"

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <ctime>
#include <QTimer>

class MpvWidget Q_DECL_FINAL : public QWidget {
    Q_OBJECT
public:
    MpvWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::Widget);
    ~MpvWidget() override;

    void command(const QVariant& params);
    void setOption(const QString &name, const QVariant &value);
    void setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name) const;
    
    void setMuted(bool mode);
    void setRepeat(bool mode);
    bool muted();
    int volume();
    void setVolume(int vol);

signals:
    void durationChanged(int value);
    void positionChanged(int value);
    void videoPaused(bool);
    void playbackFinished();

private slots:
    void on_mpv_events();

private:
    void handle_mpv_event(mpv_event *event);
    
    mpv_handle *mpv;
};
