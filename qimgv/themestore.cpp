#include "themestore.h"

ColorScheme ThemeStore::colorScheme(ColorSchemes name) {
    BaseColorScheme base;
    QPalette p;
    switch(name) {
        case COLORS_SYSTEM:
            base.folderview_topbar = p.window().color();
            base.widget = p.window().color();
            base.widget_border = p.window().color();
            base.folderview = p.base().color();
            base.text = p.text().color();
            base.icons = p.text().color();
            base.accent = p.highlight().color();
            base.scrollbar.setHsv(p.highlight().color().hue(),
                                  qBound(0, p.highlight().color().saturation() - 20, 240),
                                  qBound(0, p.highlight().color().value() - 35, 240));
            break;
        case COLORS_LIGHT:
            base.background = "#1a1a1a";
            base.background_fullscreen = "#1a1a1a";
            base.text = "#454647";
            base.icons = "#454647";
            base.widget = "#cdcdcd";
            base.widget_border = "#c3c3c3";
            base.accent = "#adadad";
            base.folderview = "#e1e1e1";
            base.folderview_topbar = "#cdcdcd";
            base.scrollbar = "#a5a5a5";
            base.overlay_text = "#d2d2d2";
            base.overlay = "#1a1a1a";
            break;
        case COLORS_DARKBLUE:
            base.background = "#18191a";
            base.background_fullscreen = "#18191a";
            base.text = "#cdd2d7";
            base.icons = "#cdd2d7";
            base.widget = "#232629";
            base.widget_border = "#26292d";
            base.accent = "#336ca5";
            base.folderview = "#232629";
            base.folderview_topbar = "#31363b";
            base.scrollbar = "#62686e";
            base.overlay_text = "#d2d2d2";
            base.overlay = "#1a1a1a";
            break;
        case COLORS_DARK:
        case COLORS_DEFAULT:
            base.background = "#1a1a1a";
            base.background_fullscreen = "#1a1a1a";
            base.text = "#a8a8a8";
            base.icons = "#a8a8a8";
            base.widget = "#252525";
            base.widget_border = "#272727";
            base.accent = "#436082";
            base.folderview = "#242424";
            base.folderview_topbar = "#343434";
            base.scrollbar = "#5e5e5e";
            base.overlay_text = "#d2d2d2";
            base.overlay = "#1a1a1a";
            break;
    }
    return ColorScheme(base);
}

//---------------------------------------------------------------------

ColorScheme::ColorScheme() {
}

ColorScheme::ColorScheme(BaseColorScheme base) {
    setBaseColors(base);
}

void ColorScheme::setBaseColors(BaseColorScheme base) {
    background            = base.background;
    background_fullscreen = base.background_fullscreen;
    text                  = base.text;
    icons                 = base.icons;
    widget                = base.widget;
    widget_border         = base.widget_border;
    accent                = base.accent;
    folderview            = base.folderview;
    folderview_topbar     = base.folderview_topbar;
    overlay               = base.overlay;
    overlay_text          = base.overlay_text;
    scrollbar             = base.scrollbar;
    createColorVariants();
}

void ColorScheme::createColorVariants() {
    if(widget.valueF() <= 0.45f) { // dark theme
        // top bar buttons
        panel_button_hover.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMin(folderview_topbar.value() + 30, 255));
        panel_button_pressed.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMin(folderview_topbar.value() + 20, 255));
        folderview_separator.setHsv(folderview.hue(), folderview.saturation(), qMin(folderview.value() + 22, 255));
        // regular buttons - from widget bg
        button.setHsv(widget.hue(), widget.saturation(), qMin(widget.value() + 21, 255));
        button_hover    = QColor(button.lighter(112));
        button_pressed  = QColor(button.darker(112));
        scrollbar_hover = scrollbar.lighter(120);
        // text
        text_hc2 = QColor(text.lighter(118));
        text_hc1 = QColor(text.lighter(110));
        text_lc1 = QColor(text.darker(130));
        text_lc2 = QColor(text.darker(160));
    } else { // light theme; still not exactly happy with this (todo)
        // top bar buttons
        panel_button_hover.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMax(folderview_topbar.value() - 40, 0));
        panel_button_pressed.setHsv(folderview_topbar.hue(), folderview_topbar.saturation(), qMax(folderview_topbar.value() - 28, 0));
        folderview_separator.setHsv(folderview.hue(), folderview.saturation(), qMax(folderview.value() - 25, 0));
        // regular buttons - from widget bg
        button.setHsv(widget.hue(), widget.saturation(), qMax(widget.value() - 36, 0));
        button_hover    = QColor(button.lighter(106));
        button_pressed  = QColor(button.darker(106));
        scrollbar_hover = scrollbar.darker(120);
        // text
        text_hc2 = QColor(text.darker(112));
        text_hc1 = QColor(text.darker(104));
        text_lc1 = QColor(text.lighter(130));
        text_lc2 = QColor(text.lighter(160));
    }
    // misc
    input_field_focus = QColor(accent);
}

