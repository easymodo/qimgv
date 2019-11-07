#include "floatingmessageproxy.h"

FloatingMessageProxy::FloatingMessageProxy(FloatingWidgetContainer *parent) :
    container(parent),
    overlay(nullptr)
{
}

FloatingMessageProxy::~FloatingMessageProxy() {
    if(overlay)
        overlay->deleteLater();
}

void FloatingMessageProxy::showMessage(QString text, FloatingMessageIcon icon, int duration) {
    init();
    overlay->showMessage(text, icon, duration);
}

void FloatingMessageProxy::showMessage(QString text, FloatingWidgetPosition position, FloatingMessageIcon icon, int duration) {
    init();
    overlay->showMessage(text, position, icon, duration);
}

void FloatingMessageProxy::init() {
    if(overlay)
        return;
    overlay = new FloatingMessage(container);
}

