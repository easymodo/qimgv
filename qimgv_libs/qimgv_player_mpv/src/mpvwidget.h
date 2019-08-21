#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QtWidgets/QOpenGLWidget>
#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>
#include <QDebug>
#include <ctime>
#include <QSurfaceFormat>
#include <QTimer>

class MpvWidget Q_DECL_FINAL: public QOpenGLWidget {
    Q_OBJECT
public:
    MpvWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::Widget);
    ~MpvWidget() Q_DECL_OVERRIDE;
    void command(const QVariant& params);
    void setOption(const QString &name, const QVariant &value);
    void setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name) const;
    // Related to this:
    // https://github.com/gnome-mpv/gnome-mpv/issues/245
    // Let's hope this wont break more than it fixes
    int width() const {
        return static_cast<int>(QOpenGLWidget::width() * this->devicePixelRatioF());
    }
    int height() const {
        return static_cast<int>(QOpenGLWidget::height() * this->devicePixelRatioF());
    }
    void setMuted(bool mode);
    void setRepeat(bool mode);
signals:
    void durationChanged(int value);
    void positionChanged(int value);
    void videoPaused(bool);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private slots:
    void swapped();
    void on_mpv_events();
    void maybeUpdate();

private:
    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);

    mpv::qt::Handle mpv;
    mpv_opengl_cb_context *mpv_gl;
};

#endif // MPVWIDGET_H
