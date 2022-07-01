#include "mpvwidget.h"
#include <stdexcept>

static void wakeup(void *ctx) {
    QMetaObject::invokeMethod((MpvWidget*)ctx, "on_mpv_events", Qt::QueuedConnection);       
}

MpvWidget::MpvWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    mpv = mpv_create();
    if(!mpv)
        throw std::runtime_error("could not create mpv context");

    int64_t wid = parent->winId();
    mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
    
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    mpv_set_option_string(mpv, "input-cursor", "no");   // no mouse handling
    mpv_set_option_string(mpv, "cursor-autohide", "no");// no cursor-autohide, we handle that
    //mpv_set_option_string(mpv, "terminal", "yes");
    //mpv_set_option_string(mpv, "msg-level", "all=v");

    // Request hw decoding, just for testing.
    mpv::qt::set_property(mpv, "hwdec", "auto");

    //mpv::qt::set_property(mpv, "video-unscaled", "downscale-big");

    // Loop video
    setRepeat(true);

    // Unmute
    setMuted(false);
    
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_set_wakeup_callback(mpv, wakeup, this);
    
    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");    
}

MpvWidget::~MpvWidget() {
    mpv_terminate_destroy(mpv);
}

void MpvWidget::command(const QVariant& params) {
    mpv::qt::command(mpv, params);
}

void MpvWidget::setProperty(const QString& name, const QVariant& value) {
    mpv::qt::set_property(mpv, name, value);
}

QVariant MpvWidget::getProperty(const QString &name) const {
    return mpv::qt::get_property(mpv, name);
}

void MpvWidget::setOption(const QString& name, const QVariant& value) {
    mpv::qt::set_property(mpv, name, value);
}

void MpvWidget::on_mpv_events() {
    // Process all events, until the event queue is empty.
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        handle_mpv_event(event);
    }
}

void MpvWidget::handle_mpv_event(mpv_event *event) {
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property *prop = reinterpret_cast<mpv_event_property*>(event->data);
        if(strcmp(prop->name, "time-pos") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE) {
                double time = *reinterpret_cast<double*>(prop->data);
                emit positionChanged(static_cast<int>(time));
            }
        } else if(strcmp(prop->name, "duration") == 0) {
            if(prop->format == MPV_FORMAT_DOUBLE) {
                double time = *reinterpret_cast<double*>(prop->data);
                emit durationChanged(static_cast<int>(time));
            } else if(prop->format == MPV_FORMAT_NONE) {
                emit playbackFinished();
            }
        } else if(strcmp(prop->name, "pause") == 0) {
            int mode = *reinterpret_cast<int*>(prop->data);
            emit videoPaused(mode == 1);
        }
        break;
    }
    default: ;
        // Ignore uninteresting or unknown events.
    }
}

void MpvWidget::setMuted(bool mode) {
    if(mode)
        mpv::qt::set_property(mpv, "mute", "yes");
    else
        mpv::qt::set_property(mpv, "mute", "no");
}

bool MpvWidget::muted() {
    return mpv::qt::get_property_variant(mpv, "mute").toBool();
}

int MpvWidget::volume() {
    return mpv::qt::get_property_variant(mpv, "volume").toInt();
}

void MpvWidget::setVolume(int vol) {
    qBound(0, vol, 100);
    mpv::qt::set_property_variant(mpv, "volume", vol);
}

void MpvWidget::setRepeat(bool mode) {
    if(mode)
        mpv::qt::set_property(mpv, "loop-file", "inf");
    else
        mpv::qt::set_property(mpv, "loop-file", "no");
}
